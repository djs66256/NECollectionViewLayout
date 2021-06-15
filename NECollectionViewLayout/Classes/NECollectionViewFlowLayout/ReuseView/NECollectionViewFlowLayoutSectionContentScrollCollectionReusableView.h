//
//  NECollectionViewFlowLayoutSectionContentScrollCollectionReusableView.h
//  CHTCollectionViewWaterfallLayout
//
//  Created by Daniel on 2019/12/6.
//

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface NECollectionViewFlowLayoutSectionContentScrollCollectionReusableView : UICollectionReusableView <UIScrollViewDelegate, UIGestureRecognizerDelegate>

@property (nonatomic, strong) UIScrollView *scrollView;

@end

NS_ASSUME_NONNULL_END
