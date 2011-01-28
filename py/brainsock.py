import time
import zmq

def main():
    context = zmq.Context(1)

    comm = context.socket(zmq.REQ)
    comm.bind('tcp://127.0.0.1:5000')

    data = context.socket(zmq.REQ)
    data.bind('tcp://127.0.0.1:5001')

    while True:
        data.send("getState")

        print data.recv()

        time.sleep(0.01)

if __name__ == '__main__':
    main()