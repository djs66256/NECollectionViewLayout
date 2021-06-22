//
//  NECollectionViewFlowLayoutAnimator.h
//  CHTCollectionViewWaterfallLayout
//
//  Created by Daniel on 2019/12/30.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@protocol NECollectionViewFlowLayoutAnimator <NSObject>

@optional

- (nullable UICollectionViewLayoutAttributes *)layout:(UICollectionViewLayout *)layout
   initialLayoutAttributesForAppearingItemAtIndexPath:(NSIndexPath *)itemIndexPath
                                   previousAttributes:(nullable UICollectionViewLayoutAttributes *)prevAttributes;

- (nullable UICollectionViewLayoutAttributes *)layout:(UICollectionViewLayout *)layout
  finalLayoutAttributesForDisappearingItemAtIndexPath:(NSIndexPath *)itemIndexPath
                                   previousAttributes:(nullable UICollectionViewLayoutAttributes *)prevAttributes;
@end

@protocol NECollectionViewFlowLayoutElementAnimator <NSObject>

@optional

- (nullable UICollectionViewLayoutAttributes *)layout:(UICollectionViewLayout *)layout
   initialLayoutAttributesForAppearingItemAtIndexPath:(NSIndexPath *)itemIndexPath
                                          elementKind:(NSString *)elementKind
                                   previousAttributes:(nullable UICollectionViewLayoutAttributes *)prevAttributes;

- (nullable UICollectionViewLayoutAttributes *)layout:(UICollectionViewLayout *)layout
  finalLayoutAttributesForDisappearingItemAtIndexPath:(NSIndexPath *)itemIndexPath
                                          elementKind:(NSString *)elementKind
                                   previousAttributes:(nullable UICollectionViewLayoutAttributes *)prevAttributes;

@end

@interface NECollectionViewFlowLayoutScaleAnimator : NSObject <NECollectionViewFlowLayoutAnimator>

@end

NS_ASSUME_NONNULL_END
