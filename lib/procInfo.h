//
//  File: procInfo.h
//  Project: Proc Info
//
//  Created by: Patrick Wardle
//  Copyright:  2017 Objective-See
//  License:    Creative Commons Attribution-NonCommercial 4.0 International License
//

#ifndef procInfo_h
#define procInfo_h

#import <libproc.h>
#import <sys/sysctl.h>
#import <Foundation/Foundation.h>

/* CLASSES */

@class Binary;
@class Process;


/* DEFINES */

//from audit_kevents.h
#define EVENT_EXIT		1
#define	EVENT_FORK      2   
#define EVENT_EXECVE    23
#define EVENT_EXEC      27
#define EVENT_SPAWN     43190

//signature status
#define KEY_SIGNATURE_STATUS @"signatureStatus"

//signing auths
#define KEY_SIGNING_AUTHORITIES @"signingAuthorities"

//code signing id
#define KEY_SIGNATURE_IDENTIFIER @"signingIdentifier"

//file belongs to apple?
#define KEY_SIGNING_IS_APPLE @"signedByApple"

//file signed with apple dev id
#define KEY_SIGNING_IS_APPLE_DEV_ID @"signedWithDevID"

//from app store
#define KEY_SIGNING_IS_APP_STORE @"fromAppStore"

/* TYPEDEFS */

//block for library
typedef void (^ProcessCallbackBlock)(Process* _Nonnull);


/* OBJECT: PROCESS INFO */

@interface ProcInfo : NSObject

//init w/ flag
// flag dictates if CPU-intensive logic (code signing, etc) should be preformed
-(id _Nullable)init:(BOOL)goEasy;

//start monitoring
-(void)start:(ProcessCallbackBlock _Nonnull )callback;

//stop monitoring
-(void)stop;

//get list of running processes
-(NSMutableArray* _Nonnull)currentProcesses;

@end

/* OBJECT: PROCESS */

@interface Process : NSObject

/* PROPERTIES */

//pid
@property pid_t pid;

//ppid
@property pid_t ppid;

//user id
@property uid_t uid;

//type
// used by process mon
@property u_int16_t type;

//exit code
@property u_int32_t exit;

//path
@property(nonatomic, retain)NSString* _Nullable path;

//args
@property(nonatomic, retain)NSMutableArray* _Nonnull arguments;

//ancestors
@property(nonatomic, retain)NSMutableArray* _Nonnull ancestors;

//Binary object
// has path, hash, etc
@property(nonatomic, retain)Binary* _Nonnull binary;

//timestamp
@property(nonatomic, retain)NSDate* _Nonnull timestamp;

/* METHODS */

//init with a pid
// method will then (try) fill out rest of object
-(id _Nullable)init:(pid_t)processID;

//set process's path
-(void)pathFromPid;

//generate list of ancestors
-(void)enumerateAncestors;

//class method to get parent of arbitrary process
+(pid_t)getParentID:(pid_t)child;

@end

/* OBJECT: BINARY */

@interface Binary : NSObject
{
    
}

/* PROPERTIES */

//path
@property(nonatomic, retain)NSString* _Nonnull path;

//name
@property(nonatomic, retain)NSString* _Nonnull name;

//icon
@property(nonatomic, retain)NSImage* _Nonnull icon;

//file attributes
@property(nonatomic, retain)NSDictionary* _Nullable attributes;

//spotlight meta data
@property(nonatomic, retain)NSDictionary* _Nullable metadata;

//bundle
// nil for non-apps
@property(nonatomic, retain)NSBundle* _Nullable bundle;

//signing info
@property(nonatomic, retain)NSDictionary* _Nonnull signingInfo;

//entitlements
@property(nonatomic, retain)NSDictionary* _Nonnull entitlements;

//hash
@property(nonatomic, retain)NSString* _Nonnull sha256;

//identifier
// either signing id or sha256 hash
@property(nonatomic, retain)NSString* _Nonnull identifier;

//flag indicating binary belongs to Apple OS
@property BOOL isApple;

//flag indicating binary is from official App Store
@property BOOL isAppStore;

/* METHODS */

//init w/ a path
-(id _Nonnull)init:(NSString* _Nonnull)path;

/* the following methods are rather CPU-intensive
   as such, if the proc monitoring is run with the 'goEasy' option, they aren't automatically invoked
*/
 
//get an icon for a process
// for apps, this will be app's icon, otherwise just a standard system one
-(void)getIcon;

//generate signing info
// also classifies if Apple/from App Store/etc.
-(void)generateSigningInfo:(SecCSFlags)flags entitlements:(BOOL)entitlements;

//generate entitlements
// note: can also call 'generateSigningInfo' w/ 'entitlements:YES'
-(void)generateEntitlements;

//generate hash
-(void)generateHash;

//generate id
// eithersigning id, or sha256 hash
// note: will generate signing info if needed
-(void)generateIdentifier;

@end

#endif
