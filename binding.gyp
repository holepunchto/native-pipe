{
  'targets': [{
    'target_name': 'sync_pipes',
    'include_dirs': [
      '<!(node -e "require(\'napi-macros\')")'
    ],
    'sources': [
      './binding.c'
    ],
  }]
}

