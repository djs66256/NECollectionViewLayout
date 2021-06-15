//
//  NECollectionViewFlowLayout+subhook.h
//  CHTCollectionViewWaterfallLayout
//
//  Created by Daniel on 2019/12/9.
//

#import "NECollectionViewFlowLayout.h"
#import "NECollectionViewFlowLayoutCalculator.h"

NS_ASSUME_NONNULL_BEGIN

@interface NECollectionViewFlowLayout (subhook)

- (NE::CollectionViewFlowLayout::Calculator&)layout;
- (CGRect)visibleRect;

@end

NS_ASSUME_NONNULL_END
