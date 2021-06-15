//
//  NEHeaderCollectionReusableView.m
//  NECollectionViewLayout_Example
//
//  Created by Daniel on 2019/12/9.
//  Copyright © 2019 Daniel. All rights reserved.
//

#import "NEHeaderCollectionReusableView.h"

@implementation NEHeaderCollectionReusableView

- (instancetype)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        _textLabel = [UILabel new];
        _textLabel.textAlignment = NSTextAlignmentCenter;
        [self addSubview:_textLabel];
    }
    return self;
}

- (void)layoutSubviews {
    [super layoutSubviews];
    _textLabel.frame = self.bounds;
}

@end
