# NECollectionViewFlowLayout

几乎等价于UICollectionViewFlowLayout，但对其增加了一些额外的能力。

## 对齐

对齐方式可以根据section来配置了。

目前支持的对齐有：（水平方向 / 竖直方向）

- 居左 / 居顶
- 居右 / 居底
- 居中 / 居中
- 两端对齐

## 背景

section可以单独设置背景view了

![](./background.png)

## Pin

目前支持更丰富的Pin能力。并且可以对单独section设置

- inside section 和系统行为一致，在section内部pin
- before section 在小于等于section的位置永远展示
- after section 在大于等于section的位置永远展示
- always 无论何时，永远展示，类似于某些列表头部的悬停区域

## 横向滚动

类似于app store的结构。如果以前要做列表内的横向列表，需要在cell上添加列表这样的双层结构才能实现，现改为一个CollectionView来实现该能力。这样：

- 减少了层次结构，减少复杂度，更符合结构上的分层
- 减少了因多列表产生的offset、性能等问题
- 可以完美接入cell的display事件

#### Page

横向滚动可以启用page功能，会变成分页效果，可以自定义分页大小 pageSize

## 性能优化

增量更新，系统layout会全量拉取size并计算，这里优化了这种情况。

如果要启用增量更新的特性，需要将`UICollectionView`替换为`NEOptimizeCollectionView`。

并且使用支持更新协议`NEOptimizeCollectionViewLayoutProtocol`的layout。

否则增量更新特性会失效，并回到全量更新策略。

## Author

Daniel, djs66256@163.com

## License

See the LICENSE file for more info.
