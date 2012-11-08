import os, os.path, time

SIGNAL_FILE = '/var/run/matrix_proxy/cmd'

modes = {
    '1': 'ripples',
    '2': 'chase',
    '3': 'bounce',
    '4': 'epilepsy',
    '5': 'rainbow',
    '6': 'strobe',
    '7': 'twinkle',
    '8': 'white',
    }

class Main:
    def __init__(self):
        self.current = None
        for mode, name in modes.items():
            print name
            if not os.system("killall -0 %s" % name):
                print "%s running" % name
                if self.current:
                    print "more than one process running!"
                    print "killall %s" % modes[self.current]
                self.current = mode

        if not self.current:
            self.start('1')

    def start(self, k):
        c = "/home/pi/c_client/%s </dev/null 2>/dev/null >/dev/null &" % modes[k]
        print c
        os.system(c)
        self.current = k

    def main(self):
        while 1:
            time.sleep(0.1)
            if not os.path.exists(SIGNAL_FILE): continue
            cmd = open(SIGNAL_FILE).read().strip()
            if cmd == self.current: continue
            if modes.has_key(cmd):
                new_mode = modes[cmd]
                print "switch mode to %s" % new_mode
                os.system("killall %s" % modes[self.current])
                self.start(cmd)
                self.current = cmd

if __name__ == '__main__':
    Main().main()
