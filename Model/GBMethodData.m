//
//  GBMethodData.m
//  appledoc
//
//  Created by Tomaz Kragelj on 26.7.10.
//  Copyright (C) 2010, Gentle Bytes. All rights reserved.
//

#import "GRMustache.h"
#import "GBMethodArgument.h"
#import "GBMethodData.h"

@interface GBMethodData ()

- (NSString *)selectorFromAssignedData;
- (NSString *)selectorDelimiterFromAssignedData;
- (NSString *)prefixFromAssignedData;
- (BOOL)formatTypesFromArray:(NSArray *)types toArray:(NSMutableArray *)array prefix:(NSString *)prefix suffix:(NSString *)suffix;
- (NSDictionary *)formattedComponentWithValue:(NSString *)value;
- (NSDictionary *)formattedComponentWithValue:(NSString *)value style:(NSUInteger)style href:(NSString *)href;

@end

#pragma mark -

@implementation GBMethodData

#pragma mark Initialization & disposal

+ (id)methodDataWithType:(GBMethodType)type result:(NSArray *)result arguments:(NSArray *)arguments {
	NSParameterAssert([arguments count] >= 1);
	return [[[self alloc] initWithType:type attributes:[NSArray array] result:result arguments:arguments] autorelease];
}

+ (id)propertyDataWithAttributes:(NSArray *)attributes components:(NSArray *)components {
	NSParameterAssert([components count] >= 2);	// At least one return and the name!
	NSMutableArray *results = [NSMutableArray arrayWithArray:components];
	[results removeLastObject];	// Remove ;
	GBMethodArgument *argument = [GBMethodArgument methodArgumentWithName:[components lastObject]];
	return [[[self alloc] initWithType:GBMethodTypeProperty attributes:attributes result:results arguments:[NSArray arrayWithObject:argument]] autorelease];
}

- (id)initWithType:(GBMethodType)type attributes:(NSArray *)attributes result:(NSArray *)result arguments:(NSArray *)arguments {
	self = [super init];
	if (self) {
		_methodType = type;
		_methodAttributes = [attributes retain];
		_methodResultTypes = [result retain];
		_methodArguments = [arguments retain];
		_methodSelectorDelimiter = [[self selectorDelimiterFromAssignedData] retain];
		_methodSelector = [[self selectorFromAssignedData] retain];
		_methodPrefix = [[self prefixFromAssignedData] retain];
	}
	return self;
}

#pragma mark Formatted components handling

- (NSArray *)formattedComponents {
	NSMutableArray *result = [NSMutableArray array];
	if (self.methodType == GBMethodTypeProperty) {
		// Add property keyword and space.
		[result addObject:[self formattedComponentWithValue:@"@property"]];
		[result addObject:[self formattedComponentWithValue:@" "]];
		
		// Add the list of attributes.
		if ([self.methodAttributes count] > 0) {
			__block BOOL isSetterOrGetter = NO;
			[result addObject:[self formattedComponentWithValue:@"("]];
			[self.methodAttributes enumerateObjectsUsingBlock:^(NSString *attribute, NSUInteger idx, BOOL *stop) {
				[result addObject:[self formattedComponentWithValue:attribute]];
				if ([attribute isEqualToString:@"setter"] || [attribute isEqualToString:@"getter"]) {
					isSetterOrGetter = YES;
					return;
				}
				if (isSetterOrGetter) {
					if ([attribute isEqualToString:@"="]) return;
					isSetterOrGetter = NO;
				}
				if (idx < [self.methodAttributes count]-1) {
					[result addObject:[self formattedComponentWithValue:@","]];
					[result addObject:[self formattedComponentWithValue:@" "]];
				}
			}];
			[result addObject:[self formattedComponentWithValue:@")"]];
			[result addObject:[self formattedComponentWithValue:@" "]];
		}
		
		// Add the list of resulting types, append space unless last component was * and property name.
		if (![self formatTypesFromArray:self.methodResultTypes toArray:result prefix:nil suffix:nil]) {
			[result addObject:[self formattedComponentWithValue:@" "]];
		}
		[result addObject:[self formattedComponentWithValue:self.methodSelector]];
	} else {
		// Add prefix.
		[result addObject:[self formattedComponentWithValue:(self.methodType == GBMethodTypeInstance) ? @"-" : @"+"]];
		[result addObject:[self formattedComponentWithValue:@" "]];
		
		// Add return types, then append all arguments.
		[self formatTypesFromArray:self.methodResultTypes toArray:result prefix:@"(" suffix:@")"];
		[self.methodArguments enumerateObjectsUsingBlock:^(GBMethodArgument *argument, NSUInteger idx, BOOL *stop) {
			[result addObject:[self formattedComponentWithValue:argument.argumentName]];
			if (argument.isTyped) {
				[result addObject:[self formattedComponentWithValue:@":"]];
				[self formatTypesFromArray:argument.argumentTypes toArray:result prefix:@"(" suffix:@")"];
				if (argument.argumentVar) [result addObject:[self formattedComponentWithValue:argument.argumentVar style:1 href:nil]];
			}
			if (idx < [self.methodArguments count]-1) [result addObject:[self formattedComponentWithValue:@" "]];
		}];
	}
	return result;
}

- (BOOL)formatTypesFromArray:(NSArray *)types toArray:(NSMutableArray *)array prefix:(NSString *)prefix suffix:(NSString *)suffix {
	BOOL hasValues = [types count] > 0;
	if (hasValues && prefix) [array addObject:[self formattedComponentWithValue:prefix]];
	
	__block BOOL lastCompWasPointer = NO;
	__block BOOL insideProtocol = NO;
	__block BOOL appendSpace = NO;
	[types enumerateObjectsUsingBlock:^(NSString *type, NSUInteger idx, BOOL *stop) {
		if (appendSpace) [array addObject:[self formattedComponentWithValue:@" "]];
		[array addObject:[self formattedComponentWithValue:type]];
		
		// We should not add space after last element or after pointer.
		appendSpace = YES;
		BOOL isLast = (idx == [types count] - 1);
		BOOL isPointer = [type isEqualToString:@"*"];
		if (isLast || isPointer) appendSpace = NO;
		
		// We should not add space between components of a protocol (i.e. id<ProtocolName> should be written without any space). Because we've alreay
		if (!isLast && [[types objectAtIndex:idx+1] isEqualToString:@"<"])
			insideProtocol = YES;
		else if ([type isEqualToString:@">"])
			insideProtocol = NO;
		if (insideProtocol) appendSpace = NO;
		
		lastCompWasPointer = isPointer;
	}];
	
	if (hasValues && suffix) [array addObject:[self formattedComponentWithValue:suffix]];
	return lastCompWasPointer;
}

- (NSDictionary *)formattedComponentWithValue:(NSString *)value {
	return [self formattedComponentWithValue:value style:0 href:nil];
}

- (NSDictionary *)formattedComponentWithValue:(NSString *)value style:(NSUInteger)style href:(NSString *)href {
	NSMutableDictionary *result = [NSMutableDictionary dictionaryWithCapacity:3];
	[result setObject:value forKey:@"value"];
	if (style > 0) {
		[result setObject:[NSNumber numberWithUnsignedInt:style] forKey:@"style"];
		[result setObject:[GRYes yes] forKey:@"emphasized"];
	}
	if (href) [result setObject:href forKey:@"href"];
	return result;
}

#pragma mark Helper methods

- (NSString *)selectorFromAssignedData {
	NSMutableString *result = [NSMutableString string];
	for (GBMethodArgument *argument in self.methodArguments) {
		[result appendString:argument.argumentName];
		[result appendString:self.methodSelectorDelimiter];
	}
	return result;
}

- (NSString *)selectorDelimiterFromAssignedData {
	if ([self.methodArguments count] > 1 || [[self.methodArguments lastObject] isTyped]) return @":";
	return @"";
}

- (NSString *)prefixFromAssignedData {
	switch (self.methodType) {
		case GBMethodTypeClass: return @"+";
		case GBMethodTypeInstance: return @"-";
	}
	return @"";
}

#pragma mark Overidden methods

- (void)mergeDataFromObject:(id)source {
	if (!source || source == self) return;
	GBLogDebug(@"%@: Merging data from %@...", self, source);
	NSParameterAssert([source methodType] == self.methodType);
	NSParameterAssert([[source methodSelector] isEqualToString:self.methodSelector]);
	NSParameterAssert([[source methodResultTypes] isEqualToArray:self.methodResultTypes]);

	// Use argument var names from the method that has comment. If no method has comment, just keep deafult.
	if ([source comment] && ![self comment]) {
		GBLogDebug(@"%@: Checking for difference due to comment status...", self);
		for (NSUInteger i=0; i<[self.methodArguments count]; i++) {
			GBMethodArgument *ourArgument = [[self methodArguments] objectAtIndex:i];
			GBMethodArgument *otherArgument = [[source methodArguments] objectAtIndex:i];
			if (![ourArgument.argumentVar isEqualToString:otherArgument.argumentVar]) {
				GBLogDebug(@"%@: Changing %ld. argument var name from %@ to %@...", self, i+1, ourArgument.argumentVar, otherArgument.argumentVar);
				ourArgument.argumentVar = otherArgument.argumentVar;
			}
		}
	}
	[super mergeDataFromObject:source];
}

- (NSString *)description {
	if (self.parentObject) {
		switch (self.methodType) {
			case GBMethodTypeClass:
			case GBMethodTypeInstance:
				return [NSString stringWithFormat:@"%@[%@ %@]", self.methodPrefix, self.parentObject, self.methodSelector];
			case GBMethodTypeProperty:
				return [NSString stringWithFormat:@"%@%@.%@", self.methodPrefix, self.parentObject, self.methodSelector];
		}
	}
	return self.methodSelector;
}

#pragma mark Properties

@synthesize methodType = _methodType;
@synthesize methodAttributes = _methodAttributes;
@synthesize methodResultTypes = _methodResultTypes;
@synthesize methodArguments = _methodArguments;
@synthesize methodSelector = _methodSelector;
@synthesize methodSelectorDelimiter = _methodSelectorDelimiter;
@synthesize methodPrefix = _methodPrefix;
@synthesize isRequired;

@end
