//
//  NECollectionViewFlowLayout.h
//  Pods-NECollectionViewLayout_Example
//
//  Created by Daniel on 2019/11/28.
//

#import <UIKit/UIKit.h>
#import "NEOptimizeCollectionViewLayoutProtocol.h"
#import "NECollectionViewFlowLayoutAnimator.h"

NS_ASSUME_NONNULL_BEGIN

typedef NS_ENUM(NSInteger, NECollectionViewFlowLayoutAlignment) {
    NECollectionViewFlowLayoutAlignLeading,     // left or top
    NECollectionViewFlowLayoutAlignTrailing,    // right or bottom
    NECollectionViewFlowLayoutAlignCenter,      // in the center of container
    NECollectionViewFlowLayoutAlignSpacingBetween,  // leading & trailing with same spacing between items
};

static inline NSString *NECollectionViewFlowLayoutAlignmentToReadable(NECollectionViewFlowLayoutAlignment alignment) {
    switch (alignment) {
        case NECollectionViewFlowLayoutAlignLeading: return @"leading";
        case NECollectionViewFlowLayoutAlignTrailing: return @"trailing";
        case NECollectionViewFlowLayoutAlignCenter: return @"center";
        case NECollectionViewFlowLayoutAlignSpacingBetween: return @"spacing between";
        default: return @"unknow";
    }
}

typedef NS_ENUM(NSInteger, NECollectionViewFlowLayoutPinToVisibleBounds) {
    NECollectionViewFlowLayoutPinToVisibleBoundsNone,           // No pinning
    NECollectionViewFlowLayoutPinToVisibleBoundsInsideSection,  // Pin inside section, the same as UICollectionViewFlowLayout
    NECollectionViewFlowLayoutPinToVisibleBoundsAfterSection,   // Pin at its section and after its sections, use by header normally.
    NECollectionViewFlowLayoutPinToVisibleBoundsBeforeSection,  // Pin at its section and before its sections, use by footer normally.
    NECollectionViewFlowLayoutPinToVisibleBoundsAlways,         // Always pin at collection view. MUST be only one, or its behavior is undefined.
};

static inline NSString *NECollectionViewFlowLayoutPinToVisibleBoundsToReadable(NECollectionViewFlowLayoutPinToVisibleBounds type) {
    switch (type) {
        case NECollectionViewFlowLayoutPinToVisibleBoundsNone: return @"none";
        case NECollectionViewFlowLayoutPinToVisibleBoundsInsideSection: return @"inside section";
        case NECollectionViewFlowLayoutPinToVisibleBoundsAfterSection: return @"after section";
        case NECollectionViewFlowLayoutPinToVisibleBoundsBeforeSection: return @"before section";
        case NECollectionViewFlowLayoutPinToVisibleBoundsAlways: return @"always";
        default: return @"unknow";
    }
}

#ifdef __cplusplus
extern "C" {
#endif

UIKIT_EXTERN NSString *const NECollectionElementKindSectionBackground;

#ifdef __cplusplus
}
#endif

@protocol NECollectionViewDelegateFlowLayout <UICollectionViewDelegateFlowLayout>
@optional

- (void)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewLayout *)layout scrollViewDidScrollToContentOffset:(CGPoint)contentOffset forSectionAtIndex:(NSInteger)section;

#pragma mark - Zindex
/// Addtion z index for all item in the section. For example, default 0 will be ( 0 + addition ) zIndex.
/// May used for more complex custom layout.
- (NSInteger)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewLayout *)layout additionZIndexForSectionAtIndex:(NSInteger)section;

#pragma mark -  Pin to visible bounds
/// The section header pin to visible bounds config.
- (NECollectionViewFlowLayoutPinToVisibleBounds)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewLayout *)layout headerPinToVisibleBoundsForSectionAtIndex:(NSInteger)section;
/// The section footer pin to visible bounds config.
- (NECollectionViewFlowLayoutPinToVisibleBounds)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewLayout *)layout footerPinToVisibleBoundsForSectionAtIndex:(NSInteger)section;

#pragma mark - Section background support
/// Show a background view in the section below the items.
/// Once return YES, you MUST return a supplementary view of NECollectionElementKindSectionBackground.
- (BOOL)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewLayout *)layout backgroundVisibleForSectionAtIndex:(NSInteger)section;

/// Define the background size. If return YES, the size will contains the header & footer.
/// Otherwise it is just the items size, without SectionInsets.
- (BOOL)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewLayout *)layout backgroundIncludeSupplementarysForSectionAtIndex:(NSInteger)section;

/// This method gives you a change to modify the background size that calculate by above configs.
- (UIEdgeInsets)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewLayout *)layout backgroundInsetsForSectionAtIndex:(NSInteger)section;

#pragma mark - Section layout alignment

/// Declare the horizontal aligment of a line in the section.
- (NECollectionViewFlowLayoutAlignment)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewLayout *)layout alignHorizontalForSectionAtIndex:(NSInteger)section;

/// Declare the vertical aligment of a line in the section.
- (NECollectionViewFlowLayoutAlignment)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewLayout *)layout alignVerticalForSectionAtIndex:(NSInteger)section;

#pragma mark - Section scroll direction and page
/// Declare the scroll direction of the section. Default Vertical.
- (UICollectionViewScrollDirection)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewLayout *)layout scrollDirectionForSectionAtIndex:(NSInteger)section;

/// Declare the content height of the section, when scroll direction Horizontal. Default 0.
/// Return 0 means use items max height, and location in one line.
/// When is not 0, layout will effect by vertical alignment and horizontal alignment.
- (CGFloat)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewLayout *)layout heightForScrollHorizontalSectionAtIndex:(NSInteger)section;

/// Enable page scroll in the section.
- (BOOL)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewLayout *)layout pageEnableForSectionAtIndex:(NSInteger)section;

/// Decide the page width of the scroll view. Default CGSizeZero.
/// Zero means the page size is equal to the frame.
/// Height is no meaning now, for preversed usage.
- (CGSize)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewLayout *)layout pageSizeForSectionAtIndex:(NSInteger)section;

@end

@interface NECollectionViewFlowLayout : UICollectionViewLayout <NEOptimizeCollectionViewLayoutProtocol>

@property (nonatomic) UICollectionViewScrollDirection sectionScrollDirection;   // Define the section layout direction.
@property (nonatomic) CGFloat sectionWidth;     // Default 0, means auto, will use container's width.
@property (nonatomic) CGFloat sectionSpacing;   // Default 0, the spacing between sections, not include section insets.

@property (nonatomic) NECollectionViewFlowLayoutAlignment alignHorizontal;  // Default leading(left).
@property (nonatomic) NECollectionViewFlowLayoutAlignment alignVertical;    // Default center.

@property (nonatomic) CGFloat minimumLineSpacing;
@property (nonatomic) CGFloat minimumInteritemSpacing;
@property (nonatomic) CGSize itemSize;

@property (nonatomic) CGSize headerReferenceSize;
@property (nonatomic) CGSize footerReferenceSize;
@property (nonatomic) UIEdgeInsets sectionInset;

@property (nonatomic, assign) BOOL pinToVisibleBoundsEnable; // Enable for pinning feature. Default NO.

@property (nonatomic, strong) id<NECollectionViewFlowLayoutAnimator> appearenceAnimator;                        // Animator for items
@property (nonatomic, strong) id<NECollectionViewFlowLayoutElementAnimator> supplementaryAppearenceAnimator;    // Animator for supplementaries
@property (nonatomic, strong) id<NECollectionViewFlowLayoutElementAnimator> decorationAppearenceAnimator;       // Animator for decoration views

/// It will close the optimization feature when setting YES.
@property (nonatomic) BOOL invalidateFlowLayoutDelegateMetricsWhenUpdates;  // Default NO.

/// Get the current contentOffset of the section. If the section is not scroll horizontal, the value is meaningless.
/// It MUST call after [collectionView layoutIfNeeded]
- (CGPoint)contentOffsetForSectionAtIndex:(NSInteger)section;
/// Set the current contentOffset of the section. If the section is not scroll horizontal, do nothing.
/// It MUST call after [collectionView layoutIfNeeded]
- (void)setContentOffset:(CGPoint)contentOffset forSectionAtIndex:(NSInteger)section;

/// Get the frame of the section.
- (CGRect)frameForSectionAtIndex:(NSInteger)section;

@end

NS_ASSUME_NONNULL_END
