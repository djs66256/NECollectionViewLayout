//
//  NECollectionViewFlowLayoutSectionContentScrollCollectionReusableView.m
//  CHTCollectionViewWaterfallLayout
//
//  Created by Daniel on 2019/12/6.
//

#import "NECollectionViewFlowLayoutSectionContentScrollCollectionReusableView.h"
#import "NECollectionViewFlowLayoutAttributes.h"

@interface NECollectionViewFlowLayoutSectionContentScrollCollectionReusableViewScrollView : UIScrollView <UIGestureRecognizerDelegate>

@end

@implementation NECollectionViewFlowLayoutSectionContentScrollCollectionReusableViewScrollView

- (BOOL)gestureRecognizer:(UIGestureRecognizer *)gestureRecognizer shouldRequireFailureOfGestureRecognizer:(UIGestureRecognizer *)otherGestureRecognizer {
    CGPoint point = [gestureRecognizer locationInView:self];
    BOOL contain = CGRectContainsPoint(self.bounds, point);
    return !contain;
}

@end

@implementation NECollectionViewFlowLayoutSectionContentScrollCollectionReusableView {
    CGSize _pageSize;
    BOOL _pageEnable;
    NSIndexPath *_indexPath;
    id<NECollectionViewFlowLayoutAttributesDelegate> _delegate;
}

- (void)dealloc {
    if (self.superview) {
        [self.superview removeGestureRecognizer:_scrollView.panGestureRecognizer];
    }
}

- (instancetype)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        self.userInteractionEnabled = NO;
        
        _scrollView = [[NECollectionViewFlowLayoutSectionContentScrollCollectionReusableViewScrollView alloc] initWithFrame:(CGRect){ CGPointZero, frame.size }];
        _scrollView.scrollsToTop = NO;
        _scrollView.backgroundColor = [UIColor clearColor];
        _scrollView.alwaysBounceHorizontal = YES;
        _scrollView.showsVerticalScrollIndicator = NO;
        _scrollView.showsHorizontalScrollIndicator = NO;
        _scrollView.directionalLockEnabled = YES;
        _scrollView.delegate = self;
        _scrollView.decelerationRate = UIScrollViewDecelerationRateFast;
        if (@available(iOS 11.0, *)) {
            _scrollView.contentInsetAdjustmentBehavior = UIScrollViewContentInsetAdjustmentNever;
        }
        if (@available(iOS 13.0, *)) {
            _scrollView.automaticallyAdjustsScrollIndicatorInsets = NO;
        }
        [self addSubview:_scrollView];
    }
    return self;
}

- (void)layoutSubviews {
    [super layoutSubviews];
    _scrollView.frame = self.bounds;
}

- (void)didMoveToSuperview {
    UIView *superview = self.superview;
    if (superview) {
        [superview addGestureRecognizer:_scrollView.panGestureRecognizer];
    }
    else {
        [superview removeGestureRecognizer:_scrollView.panGestureRecognizer];
    }
}

- (void)applyLayoutAttributes:(UICollectionViewLayoutAttributes *)layoutAttributes {
    NSParameterAssert([layoutAttributes isKindOfClass:NECollectionViewFlowLayoutAttributes.class]);
    [super applyLayoutAttributes:layoutAttributes];
    _delegate = nil;
    NECollectionViewFlowLayoutAttributes *attr = (NECollectionViewFlowLayoutAttributes *)layoutAttributes;
    if (fabs(_scrollView.contentSize.width - attr.contentSize.width) > 0.1 ) {
        _scrollView.contentSize = attr.contentSize;
    }
    
    if (fabs(_scrollView.contentOffset.x - attr.contentOffset.x) > 0.1 ) {
        _scrollView.contentOffset = attr.contentOffset;
    }
    
    _pageEnable = attr.pageEnable;
    _pageSize = attr.pageSize;
    _indexPath = attr.indexPath;
    _delegate = attr.delegate;
}

- (void)scrollViewDidScroll:(UIScrollView *)scrollView {
    [_delegate collectionViewFlowLayoutAttributesSectionDidScrollWithContentOffset:scrollView.contentOffset atIndexPath:_indexPath];
}

- (void)scrollViewWillEndDragging:(UIScrollView *)scrollView withVelocity:(CGPoint)velocity targetContentOffset:(inout CGPoint *)targetContentOffset {
    if (_pageEnable) {
        CGSize pageSize = _pageSize.width < 1 ? scrollView.frame.size : _pageSize;
        if (velocity.x > 0.1) {
            CGFloat pageNum = ceil(scrollView.contentOffset.x / pageSize.width);
            targetContentOffset->x = pageSize.width * pageNum;
        }
        else if (velocity.x < -0.1) {
            CGFloat pageNum = floor(scrollView.contentOffset.x / pageSize.width);
            targetContentOffset->x = pageSize.width * pageNum;
        }
        else {
            targetContentOffset->x = pageSize.width * round(scrollView.contentOffset.x / pageSize.width);
        }
    }
}

@end
