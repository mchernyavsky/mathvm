#!/usr/bin/env python2.7

import re
import sys
from common import *


disabled_tests = [
    # 'mark4/ackermann',
    # 'mark4/ackermann_closure',
    # 'mark4/fib',
    # 'mark4/fib_closure',
    # 'mark4/extra/.*',

    'mark4/generated/.*',
    'mark5/extra/.*',
    'perf/.*',
]


def run_test(mvm, test_dir, test):
    try:
        print 'Running test "' + test + '"...',
        sys.stdout.flush()

        test_file = os.path.join(test_dir, test + '.mvm')
        expect_file = os.path.join(test_dir, test + '.expect')

        result_data = run_mvm(mvm, '-i', test_file)
        expect_data = read_file(expect_file)

        if expect_data == result_data:
            print 'PASSED'
            return True
        else:
            print 'FAILED'
            print_result(expect_file, result_data, True)
            return False
    except KeyboardInterrupt:
        return False
    except Exception as e:
        print '\nFAILED:', e.message
    return False


if __name__ == '__main__':
    passed = []
    failed = []
    disabled = []

    for mvm, test_dir, test in load_tests():
        if any(re.match(regex, test) for regex in disabled_tests):
            disabled.append(test)
            continue

        if run_test(mvm, test_dir, test):
            passed.append(test)
        else:
            failed.append(test)

    print '\n\n Summary:', len(passed), 'PASSED,', len(failed), 'FAILED,', len(disabled), 'DISABLED'
    print '\nPASSED:', passed
    print '\nFAILED:', failed
    print '\nDISABLED:', disabled
    print '\n\n'
