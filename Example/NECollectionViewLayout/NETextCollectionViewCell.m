//
//  NETextCollectionViewCell.m
//  NECollectionViewLayout_Example
//
//  Created by Daniel on 2019/12/4.
//  Copyright © 2019 Daniel. All rights reserved.
//

#import "NETextCollectionViewCell.h"

@implementation NETextCollectionViewCell

- (instancetype)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        _textLabel = [UILabel new];
        _textLabel.font = [UIFont systemFontOfSize:17];
        _textLabel.textAlignment = NSTextAlignmentCenter;
        [self.contentView addSubview:_textLabel];
    }
    return self;
}

- (void)layoutSubviews {
    [super layoutSubviews];
    
    _textLabel.frame = self.bounds;
}

- (void)touchesBegan:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    [super touchesBegan:touches withEvent:event];
    NSLog(@"%s", sel_getName(_cmd));
}

@end
