import deviceserver

class PrinterServer(deviceserver.DeviceTCPHandler):
    def handle_script_event(self, print_width: int, param2: int, timestamp: int) -> int:
        print_data = self.req_read_seg_mem(0, 0, print_width)

        printed_text = ''
        for c in print_data:
            if c == 0x9B:
                break;

            c &= 0x7F

            if c >= 0x20 and c < 0x7F:
                printed_text += chr(c)
            else:
                printed_text += '.'

        print("Printed: {}".format(printed_text))

        return 0

if __name__ == '__main__':
    deviceserver.run_deviceserver(PrinterServer)
