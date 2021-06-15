//
//  NECollectionViewFlowLayoutAnimator.m
//  CHTCollectionViewWaterfallLayout
//
//  Created by Daniel on 2019/12/30.
//

#import "NECollectionViewFlowLayoutAnimator.h"

@implementation NECollectionViewFlowLayoutScaleAnimator

- (UICollectionViewLayoutAttributes *)layout:(UICollectionViewLayout *)layout
initialLayoutAttributesForAppearingItemAtIndexPath:(NSIndexPath *)itemIndexPath {
    UICollectionViewLayoutAttributes *attributes = [layout layoutAttributesForItemAtIndexPath:itemIndexPath].copy;
    attributes.zIndex --;
    attributes.alpha = 0.3;
    attributes.transform = CGAffineTransformScale(CGAffineTransformIdentity, 0.1, 0.1);
    return attributes;
}

- (UICollectionViewLayoutAttributes *)layout:(UICollectionViewLayout *)layout
finalLayoutAttributesForDisappearingItemAtIndexPath:(NSIndexPath *)itemIndexPath
                          previousAttributes:(UICollectionViewLayoutAttributes *)prevAttributes {
    UICollectionViewLayoutAttributes *attributes = prevAttributes.copy;
    attributes.zIndex --;
    attributes.alpha = 0.3;
    attributes.transform = CGAffineTransformScale(CGAffineTransformIdentity, 0.1, 0.1);
    return attributes;
}

@end
