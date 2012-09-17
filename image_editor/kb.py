import termios, fcntl, sys, os

# from http://stackoverflow.com/questions/983354/how-do-i-make-python-to-wait-for-a-pressed-key
class KB:
    def __init__(self):
        fd = sys.stdin.fileno()
        # save old state
        self.flags_save = fcntl.fcntl(fd, fcntl.F_GETFL)
        self.attrs_save = termios.tcgetattr(fd)
        # make raw - the way to do this comes from the termios(3) man page.
        attrs = list(self.attrs_save) # copy the stored version to update
        # iflag
        attrs[0] &= ~(termios.IGNBRK | termios.BRKINT | termios.PARMRK
                      | termios.ISTRIP | termios.INLCR | termios.IGNCR
                      | termios.ICRNL | termios.IXON)
        # oflag
        attrs[1] &= ~termios.OPOST
        # cflag
        attrs[2] &= ~(termios.CSIZE | termios.PARENB)
        attrs[2] |= termios.CS8
        # lflag
        attrs[3] &= ~(termios.ECHONL | termios.ECHO | termios.ICANON
                      #| termios.ISIG
                      | termios.IEXTEN)
        termios.tcsetattr(fd, termios.TCSANOW, attrs)
        # turn off non-blocking
        fcntl.fcntl(fd, fcntl.F_SETFL, self.flags_save & ~os.O_NONBLOCK)

    def restore(self):
        # restore old state
        fd = sys.stdin.fileno()
        termios.tcsetattr(fd, termios.TCSAFLUSH, self.attrs_save)
        fcntl.fcntl(fd, fcntl.F_SETFL, self.flags_save)

    def read_single_keypress(self, blocking=1):
        """Waits for a single keypress on stdin.

        This is a silly function to call if you need to do it a lot because it has
        to store stdin's current setup, setup stdin for reading single keystrokes
        then read the single keystroke then revert stdin back after reading the
        keystroke.

        Returns the character of the key that was pressed (zero on
        KeyboardInterrupt which can happen when a signal gets handled)

        """
        # read a single keystroke
        try:
            fd = sys.stdin.fileno()
            fcntl.fcntl(fd, fcntl.F_SETFL, self.flags_save & ~os.O_NONBLOCK if blocking else self.flags_save | os.O_NONBLOCK)
            try:
                ret = sys.stdin.read(1) # returns a single character
            except IOError:
                return None
        except KeyboardInterrupt:
            ret = 0
        return ret
kb = KB()
read = kb.read_single_keypress
restore = kb.restore
