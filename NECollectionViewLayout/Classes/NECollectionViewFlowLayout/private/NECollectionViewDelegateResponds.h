//
//  NECollectionViewDelegateResponds.h
//  Pods
//
//  Created by Daniel on 2019/11/28.
//

#ifndef NECollectionViewDelegateResponds_h
#define NECollectionViewDelegateResponds_h

#include <UIKit/UIKit.h>
#include "NECollectionViewFlowLayoutTypes.h"
#include <optional>

namespace NE {
namespace CollectionViewFlowLayout {

/// Cache of delegate respondsToSelector
struct UICollectionViewFlowLayoutResponds {
    BOOL sizeForItemAtIndexPath = NO;
    BOOL insetForSectionAtIndex = NO;
    BOOL minimumLineSpacingForSectionAtIndex = NO;
    BOOL minimumInteritemSpacingForSectionAtIndex = NO;
    BOOL referenceSizeForHeaderInSection = NO;
    BOOL referenceSizeForFooterInSection = NO;
    
    UICollectionViewFlowLayoutResponds(id delegate) :
    sizeForItemAtIndexPath([delegate respondsToSelector:@selector(collectionView:layout:sizeForItemAtIndexPath:)]),
    insetForSectionAtIndex([delegate respondsToSelector:@selector(collectionView:layout:insetForSectionAtIndex:)]),
    minimumLineSpacingForSectionAtIndex([delegate respondsToSelector:@selector(collectionView:layout:minimumLineSpacingForSectionAtIndex:)]),
    minimumInteritemSpacingForSectionAtIndex([delegate respondsToSelector:@selector(collectionView:layout:minimumInteritemSpacingForSectionAtIndex:)]),
    referenceSizeForHeaderInSection([delegate respondsToSelector:@selector(collectionView:layout:referenceSizeForHeaderInSection:)]),
    referenceSizeForFooterInSection([delegate respondsToSelector:@selector(collectionView:layout:referenceSizeForFooterInSection:)])
    { }
};

struct NECollectionViewFlowLayoutResponds : UICollectionViewFlowLayoutResponds {
public:
    NECollectionViewFlowLayoutResponds(id delegate):
    UICollectionViewFlowLayoutResponds(delegate),
    delegate_(delegate)
    {}
    
#define RespondsList(L)                         \
L(additionZIndexForSectionAtIndex)              \
\
L(headerPinToVisibleBoundsForSectionAtIndex)    \
L(footerPinToVisibleBoundsForSectionAtIndex)    \
\
L(backgroundVisibleForSectionAtIndex)           \
L(backgroundIncludeSupplementarysForSectionAtIndex) \
L(backgroundInsetsForSectionAtIndex)            \
\
L(alignHorizontalForSectionAtIndex)             \
L(alignVerticalForSectionAtIndex)               \
\
L(scrollDirectionForSectionAtIndex)             \
L(pageEnableForSectionAtIndex)                  \
L(pageSizeForSectionAtIndex)                    \
L(heightForScrollHorizontalSectionAtIndex)

#define RespondsGetter(method_name) \
bool method_name() {                \
    if (method_name ## _) {         \
        return *method_name ## _;   \
    }                               \
    else {                          \
        bool r = [delegate_ respondsToSelector:@selector(collectionView:layout:method_name:)]; \
        method_name ## _ = r;       \
        return r;                   \
    }                               \
}                                   \

    RespondsList(RespondsGetter)
    
#undef RespondsGetter
    
private:
    __weak id delegate_;
    
#define RespondsOptional(method_name) std::optional<bool> method_name ## _ = std::nullopt;
    RespondsList(RespondsOptional)
#undef RespondsOptional
    
#undef RespondsList
};
}
}

#endif /* NECollectionViewDelegateResponds_h */
