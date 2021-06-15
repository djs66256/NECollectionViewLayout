#
# Be sure to run `pod lib lint NECollectionViewLayout.podspec' to ensure this is a
# valid spec before submitting.
#
# Any lines starting with a # are optional, but their use is encouraged
# To learn more about a Podspec see https://guides.cocoapods.org/syntax/podspec.html
#

Pod::Spec.new do |s|
  s.name             = 'NECollectionViewLayout'
  s.version          = '0.1.0'
  s.summary          = 'NECollectionViewLayout.'
  s.homepage         = 'https://github.com/djs66256/NECollectionViewLayout'
  s.license          = { :text => 'Apache License 2.0' }
  s.author           = { 'Daniel' => 'djs66256@163.com' }
  s.source           = { :git => 'https://github.com/djs66256/NECollectionViewLayout', :tag => s.version.to_s }

  # s.resource_bundles = {
  #   'NECollectionViewLayout' => ['NECollectionViewLayout/Assets/**/*']
  # }

  s.ios.deployment_target = '9.0'
  s.source_files = 'NECollectionViewLayout/Classes/**/*'
  # s.private_header_files = 'NECollectionViewLayout/Classes/**/*.h'
  s.module_map = 'NECollectionViewLayout.modulemap'
  
  s.pod_target_xcconfig = {
    'CLANG_CXX_LANGUAGE_STANDARD' => 'c++17',
  }
  s.library = 'c++'
  s.frameworks = 'UIKit', 'CoreGraphics'
end
