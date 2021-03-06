﻿from __future__ import print_function
import wspc

try:
    c = wspc.Client('ws://localhost:9001')
    '''
    Go to http://localhost:9001 to get list of supported remote procedures
    and notifications along with their arguments
    '''

    c.callbacks.ping_event(
        lambda tick: print('ping - tick: {}'.format(tick)))

    # We can't use directly c.ping() since it's already
    # defined for ws4py.client.threadedclient.WebSocketClient
    ping_response = c.__getattr__('ping')()
    print('ping2() response: {}'.format(ping_response))

    ping_response = c.ping2()
    print('ping() response: {}'.format(ping_response['tick']))

    notify_resp = c.notify(2)
    print('notify() response: {}'.format(notify_resp))

    add_resp = c.calculate(arg1=20, arg2=5, op='add', comment='adding 20 to 5')
    print('calculate(20, 5, ''add'') response: {}'.format(add_resp))

    divide_resp = c.calculate(arg1=16, arg2=5, op='divide')
    print('calculate(16, 5, ''divide'') response: {}'.format(divide_resp))

    # We can also use *args instead of **kwargs. In that case we have to provide
    # all arguments - even optional ones - in correct order
    multiply_resp = c.calculate(16, 8, 'multiply', 'comment')
    print('calculate(16, 8, ''multiply'') response: {}'.format(multiply_resp))

    ops = ['add', 'subtract', 'multiply', 'divide']
    for i in range(10):
        print('{0} "{2}" {1} = {3}'.format(16 * i, 5 + i,
                                           ops[i % 4], c.calculate2(16 * i, 5 + i, ops[i % 4])))

    # Wrong function
    c.calclate()

    # wait for events
    c.run_forever()

except KeyboardInterrupt:
    c.close()
