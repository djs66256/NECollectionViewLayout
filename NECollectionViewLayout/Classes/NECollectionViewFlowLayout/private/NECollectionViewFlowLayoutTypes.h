//
//  NECollectionViewFlowLayoutTypes.h
//  Pods
//
//  Created by Daniel on 2019/12/3.
//

#ifndef NECollectionViewFlowLayoutTypes_h
#define NECollectionViewFlowLayoutTypes_h

namespace NE {
namespace CollectionViewFlowLayout {
enum class Alignment {
    Leading = NECollectionViewFlowLayoutAlignLeading,
    Trailing = NECollectionViewFlowLayoutAlignTrailing,
    Center = NECollectionViewFlowLayoutAlignCenter,
    SpacingBetween = NECollectionViewFlowLayoutAlignSpacingBetween
};

inline CGFloat CalculatePositionWithAlignment(Alignment align, CGFloat container, CGFloat value) {
    switch (align) {
        case Alignment::Trailing:
            return container - value;
            break;
        case Alignment::Center:
            return (container - value) / 2;
            break;
        default:
            return 0;
            break;
    }
}

enum class PinToVisibleBounds {
    None = NECollectionViewFlowLayoutPinToVisibleBoundsNone,
    InsideSection = NECollectionViewFlowLayoutPinToVisibleBoundsInsideSection,
    AfterSection = NECollectionViewFlowLayoutPinToVisibleBoundsAfterSection,
    BeforeSection = NECollectionViewFlowLayoutPinToVisibleBoundsBeforeSection,
    Always = NECollectionViewFlowLayoutPinToVisibleBoundsAlways,
};

enum class ScrollDirection {
    Horizontal = UICollectionViewScrollDirectionHorizontal,
    Vertical = UICollectionViewScrollDirectionVertical,
};
}
}

#endif /* NECollectionViewFlowLayoutTypes_h */
