//
//  GBApplicationSettingsProvider.h
//  appledoc
//
//  Created by Tomaz Kragelj on 3.10.10.
//  Copyright (C) 2010, Gentle Bytes. All rights reserved.
//

#import "GBCommentComponentsProvider.h"
#import "GBApplicationStringsProvider.h"

@class GBModelBase;

/** Main application settings provider.
 
 This object implements `GBApplicationStringsProviding` interface and is used by `GBAppledocApplication` to prepare application-wide settings including factory defaults, global and session values. The main purpose of the class is to simplify `GBAppledocApplication` class by decoupling it from the actual settings providing implementation.
 
 To create a new setting use the following check list to update `GBApplicationSettingsProvider`:
 
 1. Create the property here (don't forget about `@synthetize`!).
 2. Set default value in initializer.
 
 If the setting should be mapped to command line switch also do the following in `GBAppledocApplication`:
 
 1. Create a new global string as `static NSString` containing the command line switch name.
 2. Register the switch to `DDCli` (add negated switch if it's a boolean).
 3. Add unit test in `GBAppledocApplicationTesting.m` that validates the switch is properly mapped to setting property (note that boolean swithces require testing normal and negated variants!).
 4. Add KVC setter and map to corresponding property to make the test pass (again booleans require two setters).
 5. If the switch value uses template placeholders, add unit test in `GBApplicationSettingsProviderTesting.m` that validates the switch is handled.
 6. If previous point was used, add the code to `replaceAllOccurencesOfPlaceholderStringsInSettingsValues` to make the test pass.
 7. Add the switch value printout to `printSettingsAndArguments:`.
 8. Add the switch help printout to `printHelp`.
 */
@interface GBApplicationSettingsProvider : NSObject

///---------------------------------------------------------------------------------------
/// @name Initialization & disposal
///---------------------------------------------------------------------------------------

/** Returns autoreleased instance of the class.
 */
+ (id)provider;

///---------------------------------------------------------------------------------------
/// @name Project values handling
///---------------------------------------------------------------------------------------

/** Human readable name of the project. */
@property (copy) NSString *projectName;

/** Human readable name of the project company. */
@property (copy) NSString *projectCompany;

/** Human readable version of the project. */
@property (copy) NSString *projectVersion;

/** Company unique identifier, ussualy in the form of reverse domain like _com.company_. */
@property (copy) NSString *companyIdentifier;

/** Project identifier which is derived by normalizing `projectName`. */
@property (readonly) NSString *projectIdentifier;

/** Version identifier which is derived by normalizing `projectVersion`. */
@property (readonly) NSString *versionIdentifier;

///---------------------------------------------------------------------------------------
/// @name Documentation set handling
///---------------------------------------------------------------------------------------

/** Documentation set bundle identifier. */
@property (copy) NSString *docsetBundleIdentifier;

/** Documentation set bundle name. */
@property (copy) NSString *docsetBundleName;

/** Documentation set certificate issuer. */
@property (copy) NSString *docsetCertificateIssuer;

/** Documentation set certificate signer. */
@property (copy) NSString *docsetCertificateSigner;

/** Documentation set description. */
@property (copy) NSString *docsetDescription;

/** Documentation set fallback URL. */
@property (copy) NSString *docsetFallbackURL;

/** Documentation set feed name. */
@property (copy) NSString *docsetFeedName;

/** Documentation set feed URL. */
@property (copy) NSString *docsetFeedURL;

/** Documentation set package URL. */
@property (copy) NSString *docsetPackageURL;

/** Documentation set minimum Xcode version. */
@property (copy) NSString *docsetMinimumXcodeVersion;

/** Documentation set platform family. */
@property (copy) NSString *docsetPlatformFamily;

/** Documentation set publisher identifier. */
@property (copy) NSString *docsetPublisherIdentifier;

/** Documentation set publisher name. */
@property (copy) NSString *docsetPublisherName;

/** Documentation set human readble copyright message. */
@property (copy) NSString *docsetCopyrightMessage;

/** The name of the documentation set installed bundle. The folder is generated in `docsetInstallPath`. */
@property (copy) NSString *docsetBundleFilename;

/** The name of the documentation set atom file when generating publishing files. The file is generated in `outputPath`. */
@property (copy) NSString *docsetAtomFilename;

/** The name of the documentation set compressed package file when generating publishing files. The file is generated in `outputPath`. */
@property (copy) NSString *docsetPackageFilename;

///---------------------------------------------------------------------------------------
/// @name Paths handling
///---------------------------------------------------------------------------------------

/** The base path to template files used for generating various output files. */
@property (copy) NSString *templatesPath;

/** The base path of the generated files. */
@property (copy) NSString *outputPath;

/** The path to which documentation set is to be installed. */
@property (copy) NSString *docsetInstallPath;

/** The path to `docsetutil` tool, including tool filename. */
@property (copy) NSString *docsetUtilPath;

/** The list of all full or partial paths to be ignored. 
 
 It's recommended to check if a path string ends with any of the given paths before processing it. This should catch directory and file names properly as directories are processed first.
 */
@property (retain) NSMutableArray *ignoredPaths;

///---------------------------------------------------------------------------------------
/// @name Behavior handling
///---------------------------------------------------------------------------------------

/* Indicates whether HTML files should be generated or not.
 
 If `YES`, HTML files are generated in `outputPath` from parsed and processed data. If `NO`, input files are parsed and processed, but nothing is generated.
 
 @see createDocSet
 */
@property (assign) BOOL createHTML;

/** Specifies whether documentation set should be created from the HTML files.
 
 If `YES`, HTML files from html subdirectory in `outputPath` are moved to proper subdirectory within docset output files, then helper files are generated from parsed data. Documentation set files are also indexed. If `NO`, HTML files are left in the output path.
 
 @see createHtml
 @see installDocSet
 @see publishDocSet
 */
@property (assign) BOOL createDocSet;

/** Specifies whether the documentation set should be installed or not.
 
 If `YES`, temporary files used for indexing and removed, then documentation set bundle is created from the files from docset output path and is moved to `docsetInstallPath`. If `NO`, all documentation set files are left in output path.
 
 @see createDocSet
 @see publishDocSet
 */
@property (assign) BOOL installDocSet;

/** Specifies whether the documentation set should be prepared for publishing or not.
 
 If `YES`, installed documentation set is packaged for publishing - an atom feed is created and documentation set is archived. If the atom feed file is alreay found, it is updated with new information. Both, the feed and archived docset files are located within `outputPath`. If `NO`, documentation set is not prepared for publishing.
 
 @see createDocSet
 @see installDocSet
 */
@property (assign) BOOL publishDocSet;

/** Specifies whether intermediate files should be kept in `outputPath` or not.
 
 If `YES`, all intermediate files (i.e. HTML files and documentation set files) are kept in output path. If `NO`, only final results are kept. This setting not only affects how the files are being handled, it also affects performance. If intermediate files are not kept, appledoc moves files between various generation phases, otherwise it copies them. So it's prefferable to leave this option to `NO`. This option only affects output files, input source files are always left intact!
 */
@property (assign) BOOL keepIntermediateFiles;

/** Indicates whether the first paragraph needs to be repeated within method and property description or not.
 
 If `YES`, first paragraph is repeated in members description, otherwise not.
 */
@property (assign) BOOL repeatFirstParagraphForMemberDescription;

/* Indicates whether undocumented classes, categories or protocols should be kept or ignored when generating output.
 
 If `YES` undocumented objects are kept and are used for output generation. If `NO`, these objects are ignored, but only if all their members are also not documented - as soon as a single member is documented, the object is included in output together with all of it's documented members.
 
 @warning *Note:* Several properties define how undocumented objects are handled: `keepUndocumentedObjects`, `keepUndocumentedMembers` and `findUndocumentedMembersDocumentation`. To better understand how these work together, this is the workflow used when processing parsed objects, prior than passing them to output generators:
 
 1. If `findUndocumentedMembersDocumentation` is `YES`, all undocumented methods and properties documentation is searched for in known super class hierarchy. If documentation is found in any of the super classes, it is copied to inherited member as well. If `findUndocumentedMembersDocumentation` is `NO`, members are left undocumented and are handled that way in next steps.
 2. If `keepUndocumentedMembers` is `NO`, all parsed objects' members are iterated over. Any undocumented method or property is removed from class (of course any documentation copied over from super classes in previous step is considered valid too). If `keepUndocumentedMembers` is `NO`, all members are left and if `warnOnUndocumentedMembers` is `YES`, warnings are logged for all undocumented members.
 3. If `keepUndocumentedObjects` is `NO`, all undocumented classes, categories and protocols that have no documented method or property are also removed. If `keepUndocumentedObjects` is `NO`, all objects are left in the store and are used for output generation and if `warnOnUndocumentedObject` is `YES`, warnings are logged for all undocumented objects.
 
 @see keepUndocumentedMembers
 @see findUndocumentedMembersDocumentation;
 @see warnOnUndocumentedObject
 */
@property (assign) BOOL keepUndocumentedObjects;

/* Indicates whether undocumented methods or properties should be processed or not.
 
 If `YES`, undocumented members are still used for output generation. If `NO`, these members are ignored, as if they are not part of the object. Note that this only affects documented objects: if an object is not documented and none of it's members is documented, the object is not processed for output, even if this value is `YES`!
 
 @warning *Note:* This property works together with `keepUndocumentedObjects` and `findUndocumentedMembersDocumentation`. To understand how they are used, read documentation for `keepUndocumentedObjects`.
 
 @see keepUndocumentedObjects
 @see findUndocumentedMembersDocumentation
 @see warnOnUndocumentedMember
 */
@property (assign) BOOL keepUndocumentedMembers;

/** Specifies whether undocumented inherited methods or properties should be searched for in known places.
 
 If `YES`, any undocumented overriden method or property is searched for in known super classes and adopted protocols and if documentation is found there, it is copied over. This works great for objects which would otherwise only show class documentation and no member. It's also how Apple documentation uses. Defaults to `YES`.
 
 @warning *Note:* This property works together with `keepUndocumentedObjects` and `keepUndocumentedMembers`. To understand how they are used, read documentation for `keepUndocumentedObjects`.
 
 @see keepUndocumentedObjects
 @see keepUndocumentedMembers
 */
@property (assign) BOOL findUndocumentedMembersDocumentation;

/** Indicates whether categories should be merges to classes they extend or not.
 
 If `YES`, all methods from categories and extensions are merged to their classes. If `NO`, categories are left as independent objects in generated output. This is the main categories merging on/off switch, it merely enables or disables merging, other category merging settings define how exactly the methods from categories and extensions are merged into their classes.
 
 Default value is `YES` and should be left so as this seems to be the way Apple has it's documentation generated.
 
 @warning *Important:* Only categories for known project classes are merged. Categories to other framework classes, such as Foundation, AppKit or UIKit are not merged. In other words: only if the class source code is available on any of the given input paths, and is properly documented, it gets it's categories and extension methods merged! Also note that this option affects your documentation links - if any link is pointing to category that's going to be merged, it will be considered invalid link, so it's best to decide whther to merge categories of nor in advance and then consistently use properly formatted links.
 
 @see keepMergedCategoriesSections
 @see prefixMergedCategoriesSectionsWithCategoryName
 */
@property (assign) BOOL mergeCategoriesToClasses;

/** Indicates whether category or extension sections should be preserved when merging into extended class.
 
 If `YES`, all the sections from category or extension documentation are preserved. In such case, `prefixMergedCategoriesSectionsWithCategoryName` may optionally be used to prefix section name with category name or not. If `NO`, category or extension sections are ignored and a single section with category name is created in the class.
 
 Default value is `NO`. If you use many sections within the categories, you should probably leave this option unchanged as preserving all category sections might yield fragmented class documentation. Experiment a bit to see what works best for you.
 
 @warning *Note:* This option is ignored unless `mergeCategoriesToClasses` is used.
 
 @see prefixMergedCategoriesSectionsWithCategoryName
 @see mergeCategoriesToClasses
 */
@property (assign) BOOL keepMergedCategoriesSections;

/** Indicates whether merged section names from categories should be prefixed with category name.
 
 If `YES`, all merged section names from categories are prefixed with category name to make them more easily identifiable. If `NO`, section names are not changed. The first option is useful in case end users of your code are aware of different categories (if you're writting a framework for example). On the other hand, if you're using categories mostly as a way to split class definition to multiple files, you might want to keep this option off.
 
 @warning *Note:* This option is ignored unless `mergeCategoriesToClasses` and `keepMergedCategoriesSections` is used. The option is also ignored for extensions; only section names are used for extensions!
 
 @see keepMergedCategoriesSections
 @see mergeCategoriesToClasses
 */
@property (assign) BOOL prefixMergedCategoriesSectionsWithCategoryName;

///---------------------------------------------------------------------------------------
/// @name Warnings handling
///---------------------------------------------------------------------------------------

/** Indicates whether appledoc will warn if `--output` argument is not given.
 
 Although appledoc still generates output in current directory, it's better to warn the user as in most cases this is not what she wants (for example if appledoc is invoked from Xcode build script, current working directory might point to some unpredicted location). appledoc also writes the exact path that will be used for generating output.
 
 Note that in case documentation set is installed to Xcode, setting output path is irrelevant as all files from output are moved to locations Xcode uses for finding documentation sets.
 */
@property (assign) BOOL warnOnMissingOutputPathArgument;

/** Indicates whether appledoc will warn if `--company-id` argument is not given.
 
 Although appledoc deducts this information from other values, it's better to warn the user as deducted information doesn't necessarily produce correct results.
 
 Note that the warning is only issued if documentation set creation is requested.
 */
@property (assign) BOOL warnOnMissingCompanyIdentifier;

/** Indicates whether appledoc will warn if it encounters an undocumented class, category or protocol.
 
 @see warnOnUndocumentedMember
 */
@property (assign) BOOL warnOnUndocumentedObject;

/** Indicates whether appldoc will warn if it encounters an undocumented method or property.
 
 @see warnOnUndocumentedObject
 */
@property (assign) BOOL warnOnUndocumentedMember;

/** Indicates whether invalid cross reference should result in warning or not. */
@property (assign) BOOL warnOnInvalidCrossReference;

/** Indicates whether missing method argument descriptions in comments should result in warnings or not. */
@property (assign) BOOL warnOnMissingMethodArgument;

///---------------------------------------------------------------------------------------
/// @name Application-wide HTML helpers
///---------------------------------------------------------------------------------------

/** Returns HTML reference name for the given object.
 
 This should only be used for creating anchors that need to be referenced from other parts of the same HTML file. The method works for top-level objects as well as their members.
 
 @param object The object for which to return reference name.
 @return Returns the reference name of the object.
 @exception NSException Thrown if the given object is `nil`.
 @see htmlReferenceForObject:fromSource:
 @see htmlReferenceForObjectFromIndex:
 */
- (NSString *)htmlReferenceNameForObject:(GBModelBase *)object;

/** Returns relative HTML reference to the given object from the context of the given source object.
 
 This is useful for generating hrefs from one object HTML file to another. This is the swiss army knife king of a method for all hrefs generation. It works for any kind of links:
 
 - Index to top-level object (if source is `nil`).
 - Index to a member of a top-level object (if source is `nil`).
 - Top-level object to same top-level object.
 - Top-level object to a different top-level object.
 - Top-level object to one of it's members.
 - Member object to it's top-level object.
 - Member object to another top-level object.
 - Member object to another member of the same top-level object.
 - Member object to a member of another top-level object.
 
 @param object The object for which to generate the reference to.
 @param source The source object from which to generate the reference from or `nil` for index to object reference.
 @return Returns the reference string.
 @exception NSException Thrown if object is `nil`.
 @see htmlReferenceForObjectFromIndex:
 @see htmlReferenceNameForObject:
 */
- (NSString *)htmlReferenceForObject:(GBModelBase *)object fromSource:(GBModelBase *)source;

/** Returns relative HTML reference to the given object from the context of index file.
 
 This is simply a helper method for `htmlReferenceForObject:fromSource:`, passing the given object as object parameter and `nil` as source.
 
 @pram object The object for which to generate the reference to.
 @return Returns the reference string.
 @exception NSException Thrown if object is `nil`.
 @see htmlReferenceForObject:fromSource:
 @see htmlReferenceNameForObject:
 */
- (NSString *)htmlReferenceForObjectFromIndex:(GBModelBase *)object;

/** The file extension for html files.
 */
@property (readonly) NSString *htmlExtension;

///---------------------------------------------------------------------------------------
/// @name Helper methods
///---------------------------------------------------------------------------------------

/** Replaces all occurences of placeholder strings in all related values of the receiver.
 
 This message should be sent once all the values have been set. It is a convenience method that prepares all values that can use placeholder strings. From this point on, the rest of the application can simply use properties to get final values instead of sending `stringByReplacingOccurencesOfPlaceholdersInString:` all the time.
 
 Note that `stringByReplacingOccurencesOfPlaceholdersInString:` is still available for cases where placeholder strings may be used elsewhere (template files for example).
 
 @see stringByReplacingOccurencesOfPlaceholdersInString:
 */
- (void)replaceAllOccurencesOfPlaceholderStringsInSettingsValues;

/** Replaces all placeholders occurences in the given string.
 
 This method provides application-wide string placeholders replacement functionality. It replaces all known placeholders with actual values from the receiver. Placeholders are identified by a dollar mark, followed by placeholder name. The following placeholders are supported (note that case is important!):
 
 - `%PROJECT`: Replaced by `projectName` value.
 - `%PROJECTID`: Replaced by `projectIdentifier` value.
 - `%COMPANY`: Replaced by `projectCompany` value.
 - `%COMPANYID`: Replaced by `companyIdentifier` value.
 - `%VERSION`: Replaced by `projectVersion` value.
 - `%VERSIONID`: Replaced by `versionIdentifier` value.
 - `%DOCSETBUNDLEFILENAME`: Replaced by `docsetBundleFilename` value.
 - `%DOCSETATOMFILENAME`: Replaced by `docsetAtomFilename` value.
 - `%DOCSETPACKAGEFILENAME`: Replaced by `docsetPackageFilename` value.
 - `%YEAR`: Replaced by current year as four digit string.
 - `%UPDATEDATE`: Replaced by current date in the form of year, month and day with format `YYYY-MM-DD`. For example `2010-11-30`.
 
 @param string The string to replace placeholder occurences in.
 @return Returns new string with all placeholder occurences replaced.
 @see replaceAllOccurencesOfPlaceholderStringsInSettingsValues
 */
- (NSString *)stringByReplacingOccurencesOfPlaceholdersInString:(NSString *)string;

///---------------------------------------------------------------------------------------
/// @name Helper classes
///---------------------------------------------------------------------------------------

/** Returns the `GBCommentComponentsProvider` object that identifies comment components. */
@property (retain) GBCommentComponentsProvider *commentComponents;

/** Returns the `GBApplicationStringsProvider` object that specifies all string templates used for output generation. */
@property (retain) GBApplicationStringsProvider *stringTemplates;

@end
