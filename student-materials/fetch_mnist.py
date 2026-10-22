"""Download official CVDF files, decompress and verify expected SHA-256."""
from pathlib import Path
import urllib.request,gzip,hashlib,json,argparse

def fetch(destination,verify_remote=False):
    expected=json.loads((Path(__file__).parent/'MNIST-SHA256.json').read_text())
    destination=Path(destination);destination.mkdir(parents=True,exist_ok=True);report={}
    for name,checksum in expected.items():
        path=destination/name;url='https://storage.googleapis.com/cvdf-datasets/mnist/'+name+'.gz'
        if verify_remote or not path.exists():
            with urllib.request.urlopen(url,timeout=90) as response:compressed=response.read()
            raw=gzip.decompress(compressed)
            actual=hashlib.sha256(raw).hexdigest()
            if actual!=checksum:raise ValueError('Downloaded checksum mismatch: '+name)
            if not path.exists():path.write_bytes(raw)
        actual=hashlib.sha256(path.read_bytes()).hexdigest()
        if actual!=checksum:raise ValueError('Local checksum mismatch: '+name)
        report[name]=dict(url=url,sha256=actual,remote_compared=verify_remote)
    return report

if __name__=='__main__':
    p=argparse.ArgumentParser();p.add_argument('destination');p.add_argument('--verify-remote',action='store_true');args=p.parse_args()
    print(json.dumps(fetch(args.destination,args.verify_remote),indent=2))
