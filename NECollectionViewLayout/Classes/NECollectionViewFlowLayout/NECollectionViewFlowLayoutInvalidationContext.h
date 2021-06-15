//
//  NECollectionViewFlowLayoutInvalidationContext.h
//  CHTCollectionViewWaterfallLayout
//
//  Created by Daniel on 2019/12/3.
//

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface NECollectionViewFlowLayoutInvalidationContext : UICollectionViewFlowLayoutInvalidationContext

#pragma mark - Section scroll support

/// Invalidate the contentOffset of the section
/// @param offset contentOffset of the section
/// @param index the index of section
- (void)invalidateScrollOffset:(CGPoint)offset forSectionAtIndex:(NSInteger)index;

/// All invalidated contentOffset section infos.
@property (nonatomic, readonly) NSDictionary<NSNumber *, NSValue *> *invalidatedSectionScrollOffsets;

@end

NS_ASSUME_NONNULL_END
