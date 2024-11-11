import serial
import time

class UARTPC:
    def __init__(self, port='COM7', baudrate=2400):
        self.ser = serial.Serial(port, baudrate, timeout=2)

    def send_data(self, data):
        print("Starting data transmission...")
        start_time = time.time()  # Start the timer

        # Send data character by character
        for char in data:
            self.ser.write(char.encode())
            time.sleep(0.05)  # Delay per character to avoid buffer overflow
        self.ser.write(b"\0")  # Send null character

        end_time = time.time()  # End the timer
        total_time = end_time - start_time
        bytes_sent = len(data) + 1  # Total bytes sent including null character
        per_byte_time = total_time / bytes_sent

        print("Data transmission complete.")
        print(f"Total time taken for data transmission: {total_time:.4f} seconds")
        print(f"Per byte transmission time: {per_byte_time:.4f} seconds")

    def receive_data(self):
        print("Receiving data from microcontroller...")
        received_data = bytearray()
        while True:
            if self.ser.in_waiting > 0:
                received_data.extend(self.ser.read(self.ser.in_waiting))
                if b"\0" in received_data:
                    received_data = received_data.replace(b"\0", b"")
                    break
            time.sleep(0.1)  # Short delay to allow data accumulation
        print("Data reception complete.")
        return received_data.decode(errors='replace')

    def close(self):
        self.ser.close()

def main():
    pc_uart = UARTPC()
    data = """Finance Minister Arun Jaitley Tuesday hit out at former RBI governor Raghuram Rajan for predicting that the next banking crisis would be triggered by MSME lending, saying postmortem is easier than taking action when it was required. Rajan, who had as the chief economist at IMF warned of impending financial crisis of 2008, in a note to a parliamentary committee warned against ambitious credit targets and loan waivers, saying that they could be the sources of next banking crisis. Government should focus on sources of the next crisis, not just the last one. In particular, government should refrain from setting ambitious credit targets or waiving loans. Credit targets are sometimes achieved by abandoning appropriate due diligence, creating the environment for future NPAs," Rajan said in the note. Both MUDRA loans as well as the Kisan Credit Card, while popular, have to be examined more closely for potential credit risk. Rajan, who was RBI governor for three years till September 2016, is currently."""

    pc_uart.send_data(data)
    received_data = pc_uart.receive_data()
    print("Received in PC from microcontroller:", received_data)
    pc_uart.close()

if __name__ == "__main__":
    main()
