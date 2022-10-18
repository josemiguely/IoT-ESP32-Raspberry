def TCP_frag_recv(conn):
    doc = b""
    while True:
        try:
            conn.settimeout(5)
            data = conn.recv(1024)
            if data == b'\0':
                break
            else:
                doc += data
        except TimeoutError:
            conn.send(b'\0')
            raise
        except Exception:
            conn.send(b'\0')
            raise
        conn.send(b'\1')
    return doc

def UDP_frag_recv(s):
    doc = b""
    addr = None
    while True:
        try:

            data, addr = s.recvfrom(1024)
            if data == b'\0':
                break
            else:
                doc += data
        except TimeoutError:
            raise
        except Exception:
            raise
        # s.sendto(b'\1', addr)
    return (doc, addr)