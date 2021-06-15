//
//  NEOptimizeCollectionView.h
//  CHTCollectionViewWaterfallLayout
//
//  Created by Daniel on 2019/12/4.
//

#import <UIKit/UIKit.h>
#import "NEOptimizeCollectionViewLayoutProtocol.h"

NS_ASSUME_NONNULL_BEGIN

/// A collection view with optimization feature. To support this feature, the layout MUST
/// conform to NEOptimizeCollectionViewLayoutProtocol, and relayout the changed area.
@interface NEOptimizeCollectionView : UICollectionView

@end

NS_ASSUME_NONNULL_END
