//
//  KSThreadProxy.h
//  Marvel
//
//  Created by Mike on 14/10/2008.
//  Copyright 2008 Karelia Software. All rights reserved.
//

#import <Cocoa/Cocoa.h>


@interface KSThreadProxy : NSProxy
{
@private
    id          _target;
    NSThread    *_thread;
}

- (id)initWithTarget:(id)target thread:(NSThread *)thread;
- (id)initWithTarget:(id)target;

@end
