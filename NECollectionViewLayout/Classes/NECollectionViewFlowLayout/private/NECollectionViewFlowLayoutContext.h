//
//  NECollectionViewFlowLayoutContext.h
//  Pods
//
//  Created by Daniel on 2019/12/3.
//

#ifndef NECollectionViewFlowLayoutContext_h
#define NECollectionViewFlowLayoutContext_h

#include "NECollectionViewDelegateResponds.h"
#import "NECollectionViewFlowLayout.h"
#import "NECollectionViewFlowLayoutAttributes.h"

namespace NE {
namespace CollectionViewFlowLayout {
    class Context {
    public:
        Context(){}
        Context(NECollectionViewFlowLayout *layout) :
        layout_(layout),
        attributesClass_([[layout class] layoutAttributesClass]),
        delegate_(static_cast<id<NECollectionViewDelegateFlowLayout>>(layout.collectionView.delegate)),
        collectionView_(layout.collectionView),
        responds_(layout.collectionView.delegate) {}
        
        NSInteger numberOfSections() {
            return collectionView_.numberOfSections;
        }
        
        NSInteger numberOfItemsInSection(NSInteger section) {
            return [collectionView_ numberOfItemsInSection:section];
        }
        
        CGSize sizeForItemAtIndexPath(NSIndexPath *indexPath) {
            if (responds_.sizeForItemAtIndexPath) {
                return [delegate_ collectionView:collectionView_ layout:layout_ sizeForItemAtIndexPath:indexPath];
            }
            else {
                return layout_.itemSize;
            }
        }
        
        UIEdgeInsets insetForSectionAtIndex(NSInteger section) {
            if (responds_.insetForSectionAtIndex) {
                return [delegate_ collectionView:collectionView_ layout:layout_ insetForSectionAtIndex:section];
            }
            else {
                return layout_.sectionInset;
            }
        }
        
        CGFloat minimumLineSpacingForSectionAtIndex(NSInteger section) {
            if (responds_.minimumLineSpacingForSectionAtIndex) {
                return [delegate_ collectionView:collectionView_ layout:layout_ minimumLineSpacingForSectionAtIndex:section];
            }
            else {
                return layout_.minimumLineSpacing;
            }
        }
        
        CGFloat minimumInteritemSpacingForSectionAtIndex(NSInteger section) {
            if (responds_.minimumInteritemSpacingForSectionAtIndex) {
                return [delegate_ collectionView:collectionView_ layout:layout_ minimumInteritemSpacingForSectionAtIndex:section];
            }
            else {
                return layout_.minimumInteritemSpacing;
            }
        }
        
        CGSize referenceSizeForHeaderInSection(NSInteger section) {
            if (responds_.referenceSizeForHeaderInSection) {
                return [delegate_ collectionView:collectionView_ layout:layout_ referenceSizeForHeaderInSection:section];
            }
            else {
                return layout_.headerReferenceSize;
            }
        }
        
        CGSize referenceSizeForFooterInSection(NSInteger section) {
            if (responds_.referenceSizeForFooterInSection) {
                return [delegate_ collectionView:collectionView_ layout:layout_ referenceSizeForFooterInSection:section];
            }
            else {
                return layout_.footerReferenceSize;
            }
        }
        
        UICollectionViewScrollDirection sectionScrollDirection() {
            return layout_.sectionScrollDirection;
        }
        
        bool isCustomSectionWidth() {
            return layout_.sectionWidth > 1;
        }
        
        CGFloat sectionWidth() {
            return layout_.sectionWidth;
        }
        
        CGFloat sectionSpacing() {
            return layout_.sectionSpacing;
        }
        
#define PropertyList(L)                         \
L(NSInteger, additionZIndexForSectionAtIndex, 0)    \
\
L(bool, backgroundVisibleForSectionAtIndex, false)                      \
L(bool, backgroundIncludeSupplementarysForSectionAtIndex, false)        \
L(UIEdgeInsets, backgroundInsetsForSectionAtIndex, UIEdgeInsetsZero)    \
\
L(Alignment, alignHorizontalForSectionAtIndex,  static_cast<Alignment>(layout_.alignHorizontal))    \
L(Alignment, alignVerticalForSectionAtIndex,  static_cast<Alignment>(layout_.alignVertical))        \
\
L(ScrollDirection, scrollDirectionForSectionAtIndex, ScrollDirection::Vertical) \
L(bool, pageEnableForSectionAtIndex, false) \
L(CGSize, pageSizeForSectionAtIndex, CGSizeZero)    \
L(CGFloat, heightForScrollHorizontalSectionAtIndex, 0.)
        
#define PropertyGetter(Type, method_name, default_value) \
        Type method_name(NSUInteger section) {\
            if (responds_.method_name()) {\
                return static_cast<Type>([delegate_ collectionView:collectionView_ layout:layout_ method_name:section]);\
            }\
            return default_value;\
        }\
        
        PropertyList(PropertyGetter)
        
#undef PropertyGetter
#undef PropertyList

        PinToVisibleBounds headerPinToVisibleBoundsForSectionAtIndex(NSUInteger section) {
            if (pinToVisibleBoundsEnable() && responds_.headerPinToVisibleBoundsForSectionAtIndex()) {
                return static_cast<PinToVisibleBounds>([delegate_ collectionView:collectionView_
                                                                          layout:layout_
                                       headerPinToVisibleBoundsForSectionAtIndex:section]);
            }
            return PinToVisibleBounds::None;
        }
        
        PinToVisibleBounds footerPinToVisibleBoundsForSectionAtIndex(NSUInteger section) {
            if (pinToVisibleBoundsEnable() && responds_.footerPinToVisibleBoundsForSectionAtIndex()) {
                return static_cast<PinToVisibleBounds>([delegate_ collectionView:collectionView_
                                                                          layout:layout_
                                       footerPinToVisibleBoundsForSectionAtIndex:section]);
            }
            return PinToVisibleBounds::None;
        }
        
        NECollectionViewFlowLayoutAttributes *cellAttributes(NSIndexPath *indexPath) {
            NECollectionViewFlowLayoutAttributes* attr = [[[layout_ class] layoutAttributesClass] layoutAttributesForCellWithIndexPath:indexPath];
            attr.delegate = (id<NECollectionViewFlowLayoutAttributesDelegate>)layout_;
            return attr;
        }
        NECollectionViewFlowLayoutAttributes *supplementaryAttributes(NSString *kind, NSIndexPath *indexPath) {
            NECollectionViewFlowLayoutAttributes* attr = [[[layout_ class] layoutAttributesClass] layoutAttributesForSupplementaryViewOfKind:kind withIndexPath:indexPath];
            attr.delegate = (id<NECollectionViewFlowLayoutAttributesDelegate>)layout_;
            return attr;
        }
        NECollectionViewFlowLayoutAttributes *decorationAttributes(NSString *kind, NSIndexPath *indexPath) {
            NECollectionViewFlowLayoutAttributes* attr = [[[layout_ class] layoutAttributesClass] layoutAttributesForDecorationViewOfKind:kind withIndexPath:indexPath];
            attr.delegate = (id<NECollectionViewFlowLayoutAttributesDelegate>)layout_;
            return attr;
        }
        
        
    private:
        __weak NECollectionViewFlowLayout *layout_ = nil;
        __weak id<NECollectionViewDelegateFlowLayout> delegate_ = nil;
        __weak UICollectionView *collectionView_ = nil;
        NECollectionViewFlowLayoutResponds responds_ = nil;
        Class attributesClass_ = Nil;
        
        bool pinToVisibleBoundsEnable() { return layout_.pinToVisibleBoundsEnable; }
    };
}
}

#endif /* NECollectionViewFlowLayoutContext_h */
