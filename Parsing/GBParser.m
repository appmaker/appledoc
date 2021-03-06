//
//  GBParser.m
//  appledoc
//
//  Created by Tomaz Kragelj on 23.7.10.
//  Copyright (C) 2010, Gentle Bytes. All rights reserved.
//

#import "GBStore.h"
#import "GBApplicationSettingsProvider.h"
#import "GBObjectiveCParser.h"
#import "GBParser.h"

@interface GBParser ()

- (void)parseDirectory:(NSString *)path;
- (void)parseFile:(NSString *)path;
- (BOOL)isPathIgnored:(NSString *)path;
- (BOOL)isFileIgnored:(NSString *)filename;
- (BOOL)isDirectoryIgnored:(NSString *)filename;
- (BOOL)isSourceCodeFile:(NSString *)path;
@property (assign) NSUInteger numberOfParsedFiles;
@property (retain) GBObjectiveCParser *objectiveCParser;
@property (retain) GBStore *store;
@property (retain) GBApplicationSettingsProvider *settings;

@end

#pragma mark -

@implementation GBParser

#pragma mark Initialization & disposal

+ (id)parserWithSettingsProvider:(id)settingsProvider {
	return [[[self alloc] initWithSettingsProvider:settingsProvider] autorelease];
}

- (id)initWithSettingsProvider:(id)settingsProvider {
	NSParameterAssert(settingsProvider != nil);
	GBLogDebug(@"Initializing parser with settings provider %@...", settingsProvider);
	self = [super init];
	if (self) {
		self.settings = settingsProvider;
		self.objectiveCParser = [GBObjectiveCParser parserWithSettingsProvider:self.settings];
	}
	return self;
}

#pragma mark File system parsing handling

- (void)parseObjectsFromPaths:(NSArray *)paths toStore:(id)store {
	NSParameterAssert(paths != nil);
	NSParameterAssert(store != nil);
	GBLogVerbose(@"Parsing objects from %u paths...", [paths count]);
	self.store = store;
	self.numberOfParsedFiles = 0;
	for (NSString *path in paths) {
		GBLogVerbose(@"Parsing '%@'...", path);
		if ([self.fileManager isPathDirectory:path]) {
			[self parseDirectory:path];
		} else {
			[self parseFile:path];
		}
	}
}

- (void)parseDirectory:(NSString *)path {	
	GBLogDebug(@"Parsing path '%@'...", path);
	if ([self isPathIgnored:path]) {
		GBLogNormal(@"Ignoring path '%@'...", path);
		return;
	}

	NSError *error = nil;
	NSArray *contents = [self.fileManager contentsOfDirectoryAtPath:path error:&error];
	if (error) {
		GBLogNSError(error, @"Failed fetching contents of '%@'!", path);
		return;
	}
	
	// First process files. Skip ignored files.
	for (NSString *subpath in contents) {
		NSString *fullPath = [path stringByAppendingPathComponent:subpath];
		if ([self.fileManager isPathDirectory:fullPath]) continue;
		if ([self isFileIgnored:subpath]) continue;
		[self parseFile:fullPath];
	}
	
	// Now process all subdirectories. Skip ignored directories.
	for (NSString *subpath in contents) {
		NSString *fullPath = [path stringByAppendingPathComponent:subpath];
		if (![self.fileManager isPathDirectory:fullPath]) continue;
		if ([self isDirectoryIgnored:subpath]) continue;
		[self parseDirectory:fullPath];
	}
}

- (void)parseFile:(NSString *)path {
	GBLogDebug(@"Parsing file '%@'...", path);
	if ([self isPathIgnored:path]) {
		GBLogNormal(@"Ignoring file '%@'...", path);
		return;
	}
	if (![self isSourceCodeFile:path]) return;	
	
	GBLogInfo(@"Parsing source code from '%@'...", path);
	NSError *error = nil;
	NSString *input = [NSString stringWithContentsOfFile:path encoding:NSUTF8StringEncoding error:&error];
	if (error) {
		GBLogNSError(error, @"Failed reading contents of file '%@'!", path);
		return;
	}
	
	[self.objectiveCParser parseObjectsFromString:input sourceFile:[path lastPathComponent] toStore:self.store];
	self.numberOfParsedFiles++;
}

- (BOOL)isPathIgnored:(NSString *)path {
	for (NSString *ignored in self.settings.ignoredPaths) {
		if ([path hasSuffix:ignored]) return YES;
	}
	return NO;
}

- (BOOL)isFileIgnored:(NSString *)filename {
	if ([filename isEqualToString:@".DS_Store"]) return YES;
	return NO;
}

- (BOOL)isDirectoryIgnored:(NSString *)filename {
	if ([filename isEqualToString:@".git"]) return YES;
	if ([filename isEqualToString:@".svn"]) return YES;
	return NO;
}

- (BOOL)isSourceCodeFile:(NSString *)path {
	NSString *extension = [path pathExtension];
	if ([extension isEqualToString:@"h"]) return YES;
	if ([extension isEqualToString:@"hh"]) return YES;
	if ([extension isEqualToString:@"m"]) return YES;
	if ([extension isEqualToString:@"mm"]) return YES;
	return NO;
}

#pragma mark Properties

@synthesize numberOfParsedFiles;
@synthesize objectiveCParser;
@synthesize settings;
@synthesize store;

@end
