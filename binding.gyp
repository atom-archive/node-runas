{
  'target_defaults': {
    'win_delay_load_hook': 'false',
    'conditions': [
      ['OS=="win"', {
        'msvs_disabled_warnings': [
          4530,  # C++ exception handler used, but unwind semantics are not enabled
          4506,  # no definition for inline function
        ],
      }],
    ],
  },
  'targets': [
    {
      'target_name': 'spawn_as_admin',
      'sources': [
        'src/main.cc',
      ],
      'include_dirs': [
        '<!(node -e "require(\'nan\')")'
      ],
      'conditions': [
        ['OS=="win"', {
          'sources': [
            'src/spawn_as_admin_win.cc',
          ],
          'libraries': [
            '-lole32.lib',
            '-lshell32.lib',
          ],
        }],
        ['OS=="mac"', {
          'sources': [
            'src/spawn_as_admin_darwin.cc',
          ],
          'libraries': [
            '$(SDKROOT)/System/Library/Frameworks/Security.framework',
          ],
        }],
        ['OS=="linux"', {
          'sources': [
            'src/spawn_as_admin_linux.cc',
          ],
        }],
      ],
    }
  ]
}
