import zmq

def main():
    context = zmq.Context(1)

    # First, connect our subscriber socket
    sock = context.socket(zmq.SUB)
    sock.bind('ipc://maw.ipc')
    sock.setsockopt(zmq.SUBSCRIBE, '')

    while True:
        msg = sock.recv()
        print msg

if __name__ == '__main__':
    main()