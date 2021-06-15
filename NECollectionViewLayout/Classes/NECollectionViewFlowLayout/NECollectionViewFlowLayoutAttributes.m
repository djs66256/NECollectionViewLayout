//
//  NECollectionViewFlowLayoutAttributes.m
//  CHTCollectionViewWaterfallLayout
//
//  Created by Daniel on 2019/12/6.
//

#import "NECollectionViewFlowLayoutAttributes.h"

@implementation NECollectionViewFlowLayoutAttributes

- (BOOL)isEqual:(id)object {
    BOOL b = [super isEqual:object];
    if (b && [object isKindOfClass:NECollectionViewFlowLayoutAttributes.class]) {
        NECollectionViewFlowLayoutAttributes *attributes = (NECollectionViewFlowLayoutAttributes *)object;
        BOOL ret = attributes.delegate == self.delegate
        && attributes.pageEnable == self.pageEnable
        && CGSizeEqualToSize(attributes.pageSize, self.pageSize)
        && CGSizeEqualToSize(attributes.contentSize, self.contentSize)
        && CGPointEqualToPoint(attributes.contentOffset, self.contentOffset)
        && attributes.pinned == self.pinned;
        
        return ret;
    }
    return NO;
}

- (id)copyWithZone:(NSZone *)zone {
    NECollectionViewFlowLayoutAttributes *attributes = [super copyWithZone:zone];
    attributes.delegate = self.delegate;
    attributes.pageEnable = self.pageEnable;
    attributes.pageSize = self.pageSize;
    attributes.contentSize = self.contentSize;
    attributes.contentOffset = self.contentOffset;
    attributes.pinned = self.pinned;
    return attributes;
}

@end
