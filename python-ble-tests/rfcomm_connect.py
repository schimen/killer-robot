import bluetooth as bt

address = "24:6F:28:36:59:FE"
channel = 1
print(f'try to connect to {address} at channel {channel} via RFCOMM')
bt.BTsocket = bt.BluetoothSocket(bt.RFCOMM)
print('connecting...')
bt.BTsocket.connect((address, channel))

message = "Hello world"
print('send {message}')
bt.BTsocket.send(message)

print('close connection')
bt.BTsocket.close()
