//
//  MainViewController.m
//  CleverStorkDemo
//
//  Created by Sunil Gowda on 4/21/12.
//  Copyright (c) 2012 CleverStork. All rights reserved.
//

#import "MainViewController.h"
#import "CleverStork.h"

@implementation MainViewController

@synthesize autoRepeat, startButton, appInfoTable;

-(id) init {
    if( self = [super initWithNibName:@"MainViewController" bundle:nil] ) {
        keys = [[NSArray arrayWithObjects:@"Current Version", @"Latest Version", @"Update Available Message", @"Minimum Version", @"Update Required Message", @"Status", @"Time", nil] retain];
    }
    return self;
}

-(void) dealloc {
    [autoRepeat release];
    [startButton release];
    [appInfoTable release];
    [super dealloc];
}

-(void) viewDidLoad {
    [super viewDidLoad];
    appInfoTable.dataSource = self;
}

#pragma mark -

#pragma mark Private Methods

-(NSString *) storkStatusString:(CleverStorkStatus)status {
    switch(status) {
        case csCurrent: return @"Current";
        case csPending: return @"Pending";
        case csUpdateAvailable: return @"Update Available";
        case csUpdateRequired: return @"Update Required";
    }
    return nil;
}

#pragma mark -

#pragma mark IBActions

-(IBAction) autoRepeatValueChanged:(id)sender {
    // TODO
}

-(IBAction) startButtonTapped:(id)sender {
    
    NSDate *start = [NSDate date];
    CleverStork *stork = [[CleverStork alloc] initWithKey:@"531c7a5c9d124384c26e" token:@"507804efabd3bcdf5db9"];
    [stork doCheck];
    
    NSDate *end = [NSDate date];
    NSTimeInterval duration = [end timeIntervalSinceDate:start];
    
    values = [NSArray arrayWithObjects:
                        [[NSBundle mainBundle] objectForInfoDictionaryKey:@"CFBundleVersion"], 
                        stork.latestVersion, 
                        stork.latestVersionDescription, 
                        stork.minimumVersion, 
                        stork.updateRequiredMessage, 
                        [self storkStatusString:stork.status], 
                        [NSString stringWithFormat:@"%.0f milliseconds", duration * 1000.0f], 
                    nil];
    [values retain];
    [appInfoTable reloadData];
    [stork release];
}

#pragma mark -
                 

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    return [values count];
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    static NSString *cellIdentifier = @"Cell";
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:cellIdentifier];
    
    if(cell == nil) {
        cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleSubtitle reuseIdentifier:cellIdentifier];
    }
    NSString *title = [keys objectAtIndex:indexPath.row];
    NSString *value = [values objectAtIndex:indexPath.row];
    
    cell.textLabel.text = title;
    cell.detailTextLabel.text = value;
    
    return cell;
}

@end
