//
//  CleverStork.h
//  CleverStork
//
//  Copyright (c) 2012 CleverStork. All rights reserved.
//

#import <Foundation/Foundation.h>

//  ===================================================================
//           !!!!!!!!!!!!   R E A D   M E    !!!!!!!!!!!!!
//  ===================================================================
//
// CleverStork is a utility to help app developers migrate their users 
// to newer versions of the app. The utility communicates with the 
// CleverStork server to fetch details on the latest version of the app. 
//
// You will also need an app key and token  to perform the check. 
//
// You can manage your app details and obtain the app key and token at
//
//                        www.cleverstork.com.
// 
//  ===================================================================


// The various results of an app version check.
typedef enum {
    csPending = 0,                  // The check has not yet been made
    csCurrent = 1,                   // The latest version of the app is currently installed
    csUpdateAvailable = 2,      // Newer version of the app is available, but not required
    csUpdateRequired = 3       // The installed version is older than the minimum version required
} CleverStorkStatus;

// Protocol definition follows interface definition
@protocol CleverStorkDelegate;

// A CleverStork object performs the many actions in the entire lifecyle 
// of an app version check and update.
@interface CleverStork : NSObject<UIAlertViewDelegate> {
    CleverStorkStatus status;
    BOOL completed;
    NSCondition *completion;
    float timeOut;
    BOOL checkFailed;
@private
    NSString *_key, *_token;
    NSDictionary *_appInfo;
    id<CleverStorkDelegate> _delegate;
    NSMutableData *_data;
    NSDate *_start;
    BOOL _deleted; // permanently disable the check - contact us at support@cleverstork.com 
    BOOL showNewVersionDescription;
}

// Result of the app version check
@property (nonatomic, readonly) CleverStorkStatus status;


// The latest version of the app
@property (nonatomic, readonly) NSString *latestVersion;
// What's new in the latest app version
@property (nonatomic, readonly) NSString *latestVersionDescription;
// The message displayed to the user when a newer version is available (a friendly wrapper around latestVersionDescription)
@property (nonatomic, readonly) NSString *updateAvailableMessage;


// The minimum required versionf of the app
@property (nonatomic, readonly) NSString *minimumVersion;
// The update message to be displayed to the user 
// when the installed version is older than the minimum version.
@property (nonatomic, readonly) NSString *updateRequiredMessage;


// enable or disable displaying an alert the first time an updated app is launched describing the new features
@property (nonatomic, assign) BOOL showNewVersionDescription; // default YES
// The message displayed to the user the first time the app is launched after an update ( a friendly wrapper around latestVersionDescription)
@property (nonatomic, readonly) NSString *newVersionDescriptionMessage;


// The timeout value for the status check. Default is 1s.
// If the request times out, the app status is optimistically
// marked as Current and checkFailed is set to true.
// It is up to the app developer to take any action if
// a request has timed out by inspecting the checkFailed value.
@property (nonatomic, assign) float timeOut; // in seconds

// Flag to indicate if the check failed (network timeout, server error, zombies attack)
@property (nonatomic, readonly) BOOL checkFailed;

// A flag and condition to wait on while the check is completed. The condition is triggered
// only when the check is made asynschronously using doCheckWithDelegate:
@property (nonatomic, readonly) BOOL completed;
@property (nonatomic, readonly) NSCondition *completion;


// Initialize the CleverStork instance with your key and token 
// available at www.cleverstork.com. It does NOT automatically
// initiate a check. You need to call one of the doCheck methods.
-(id) initWithKey:(NSString *)key token:(NSString *)token;




// Simple version check. 
// The check is done *synchronously* on the main thread.
// This method is recommeded only for simple apps.
//
// Depending on the result of the check, an alert may be displayed to the user as described below.
//
// If an update is "required" (installed version older than minimum version),
// an alert is displayed with the update message (configured as 'update message' on cleverstork.com)
// and a single "Update" button.
// On tapping the Update button, the configured update url is opened (typically to the AppStore app). 
//
// If an update is "available" but not required (installed version matches or 
// newer than minimum version), an alert is displayed with the 
// latest app version's description (configured as 'what's new' on cleverstork.com) 
// with  three buttons - "Install Update", "Remind Me Later" and "Ignore"
// If the user opts to install the update, the configured update url is opened (typically to the AppStore app). 
// if the user opts to be reminded later, nothing happens and the method returns. The same check is triggered on next launch.
// If the user opts to Ignore this update, subsequent checks return the status as csCurrent until the current version value is updated on cleverstork.com
//
// If this is the first launch after a newer version of the app has been installed,
// an alert is displayed with the new version's description (configured as 'what's new on cleverstork.com)
// with a single "OK" button. This can be disabled by setting showNewVersionDescription to NO
//
// Note that in all cases, the method returns before the alert is presented. 
// The method returns YES if the app meets the minimum version criteria. 
// The method returns NO if the app is older than the minimum version required.
// Typically, the app should continue initialization only if the method return YES.
-(BOOL) doCheck; 




// The version check is done asynchronously on a background thread. The method returns 
// immediately and app initialization can continue while the app's version details 
// are being fetched and compared to the installed version.
//
// This is the recommended method for most apps.
//
// There are two ways to be notified of completion of the check. 
// 1. With the CleverStorkDelegate methods. The cleverStork:didCompleteCheck: method is
// invoked when the check completes.
// 2. Waiting on the 'completed' condition (a property on the CleverStock object) to fire. 
// After the check is complete, the status property holds the result.
//
// If an update is required or available, you can invoke presentUpdateOptions to present
// an alert to the user with the appropriate message. 
// You can also present these messages to the user in your custom UI. The strings are available 
// as updateAvailableMessage and updateRequiredMessage properties.
//
// The delegate (optional) is not retained. The delegate is not required if you opt 
// to wait on the condition.
//
-(void) doCheckWithDelegate:(id<CleverStorkDelegate>)delegate;



// Presents an alert to the user with the appropriate message determined by:
// If an update is "required" (installed version older than minimum version),
// an alert is displayed with the update message (configured as 'update message')
// and a single "OK" button. On tapping the OK button, the configured update url 
// is opened. 
//
// If an update is "available" but not required (installed version matches or 
// newer than minimum version), an alert is displayed with the 
// latest app version's description (configured as 'what's new' on cleverstork.com) with  
// "Install Update", "Remind Me Later" and "Ingore" buttons
// If the user opts to install the update, the configured update url is opened. 
// if the user opts to be reminded later, nothing happens and the method returns. The same check is triggered on next launch.
// If the user opts to Ignore this update, subsequent checks return the status as csCurrent until the current version value is updated on cleverstork.com
//
// The delegate (optional) is not retained. In all cases, when the user selects an 
// option, the cleverStork:didAcceptUpdateOption: delegate method is called. 
// 
// Note that in all cases, the method returns before the alert is presented. 
// The method returns YES if the app meets the minimum version criteria. 
// The method returns NO if the app is older than the minimum version required.
// Typically, the app should continue initialization only if the method return YES.
// 
-(BOOL) presentUpdateOptionsWithDelegate:(id<CleverStorkDelegate>)delegate;


// Method to trigger opening the update url if you presented the update messages
// within your custom UI (did NOT use presentUpdateOptionsWithDelegate:)  
// Note: When you present the update messages within your own custom UI, please adhere 
// to the same message determination rules as described earlier. 
-(void) doUpdate;

// Method to display an alert with the new version's description (configured as 'what's new on cleverstork.com)
// and a single "OK" button.
// If showNewVersionDescription is set to NO or if the latest version's message has
// already been displayed once, the method simply returns.
-(void) checkAndShowNewVersionDescription;

@end


// Delegate methods to be implemented when using the asynchronous method of performing the 
// version check or using presentUpdateOptionsWithDelegate: to present the update messages.
@protocol CleverStorkDelegate <NSObject>

@optional

// Callback invoked when the check is complete
-(void) cleverStork:(CleverStork *)stork didCompleteCheck:(CleverStorkStatus)status;

// Callback invoked once the user opts to install (doUpdate=YES) or declines an update (doUpdate=NO)
-(void) cleverStork:(CleverStork *)stork didAcceptUpdateOption:(bool)doUpdate;

@end
