//
//  NECollectionViewFlowLayoutAttributes.h
//  CHTCollectionViewWaterfallLayout
//
//  Created by Daniel on 2019/12/6.
//

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@protocol NECollectionViewFlowLayoutAttributesDelegate <NSObject>

- (void)collectionViewFlowLayoutAttributesSectionDidScrollWithContentOffset:(CGPoint)offset atIndexPath:(NSIndexPath *)indexPath;

@end

@interface NECollectionViewFlowLayoutAttributes : UICollectionViewLayoutAttributes

@property (nonatomic, weak) id<NECollectionViewFlowLayoutAttributesDelegate> delegate;
@property (nonatomic, assign) BOOL pageEnable;
@property (nonatomic, assign) CGSize pageSize;
@property (nonatomic, assign) CGSize contentSize;
@property (nonatomic, assign) CGPoint contentOffset;

@property (nonatomic, assign) BOOL pinned;

@end

NS_ASSUME_NONNULL_END
