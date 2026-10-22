"""Student data tools: no model, training or backpropagation implementation."""
from pathlib import Path
import csv,hashlib,random,struct
import math

DIGITS=['zero','un','deux','trois','quatre','cinq','six','sept','huit','neuf']

def read_pattern(path):
    lines=Path(path).read_text().splitlines()
    if len(lines)!=9 or any(len(row)!=6 or set(row)-set('.*') for row in lines[:8]):
        raise ValueError('Expected 8 rows of 6 pixels, then one label')
    target=int(lines[8])
    if target not in range(10):raise ValueError('Invalid digit')
    return [int(p=='*') for row in lines[:8] for p in row],target

def noisy_copy(pixels,percent,rng):
    """Round to nearest integer, ties upward; sample DISTINCT indices."""
    if len(pixels)!=48 or percent not in range(0,101):raise ValueError('Invalid noise request')
    count=(48*percent+50)//100
    indices=rng.sample(range(48),count);result=pixels[:]
    for i in indices:result[i]=1-result[i]
    assert sum(a!=b for a,b in zip(result,pixels))==count
    return result

def generate_noise(pattern_dir,out,seed=2026):
    rng=random.Random(seed)
    with open(out,'w',newline='') as f:
        w=csv.writer(f);w.writerow(['noise_seed','digit','p','copy','flipped','pixels'])
        for digit,name in enumerate(DIGITS):
            pixels,label=read_pattern(Path(pattern_dir)/(name+'.txt'));assert label==digit
            for p in range(0,101,10):
                for copy in range(50):
                    result=noisy_copy(pixels,p,rng)
                    w.writerow([seed,digit,p,copy,(48*p+50)//100,''.join(map(str,result))])
    return hashlib.sha256(Path(out).read_bytes()).hexdigest()

def check_mnist(directory):
    result={}
    for prefix,n in [('train',60000),('t10k',10000)]:
        for kind,magic,size in [('images',2051,16+n*784),('labels',2049,8+n)]:
            name=f'{prefix}-{kind}-idx{3 if kind=="images" else 1}-ubyte';p=Path(directory)/name;data=p.read_bytes()
            header=struct.unpack('>4I' if kind=='images' else '>2I',data[:16 if kind=='images' else 8])
            expected=(magic,n,28,28) if kind=='images' else (magic,n)
            if header!=expected or len(data)!=size:raise ValueError('Invalid IDX: '+name)
            if kind=='labels' and max(data[8:])>9:raise ValueError('Label outside 0..9')
            result[name]=dict(header=header,bytes=len(data),sha256=hashlib.sha256(data).hexdigest())
    return result

def next_u32(state):
    state^=(state<<13)&0xffffffff;state^=state>>17;state^=(state<<5)&0xffffffff
    return state&0xffffffff

def mnist_noisy_pixels(image_file,seed=2026):
    """Same exact 78 distinct inversions per image as the C research campaign."""
    raw=Path(image_file).read_bytes()
    if struct.unpack('>4I',raw[:16])!=(2051,10000,28,28) or len(raw)!=16+10000*784:raise ValueError('Invalid test IDX')
    pixels=bytearray(raw[16:]);state=seed
    for r in range(10000):
        ids=list(range(784))
        for j in range(78):
            state=next_u32(state);k=j+state%(784-j);ids[j],ids[k]=ids[k],ids[j]
            index=784*r+ids[j];pixels[index]=255-pixels[index]
    return bytes(pixels)

def read_weights(path):
    """Return flat W1,b1,W2,b2 values. No inference or backprop provided."""
    data=Path(path).read_bytes()
    if len(data)!=318060 or data[:8]!=b'INEMMLP1' or struct.unpack('<3I',data[8:20])!=(784,100,10):raise ValueError('Invalid weights')
    values=list(struct.unpack('<79510f',data[20:]))
    if not all(math.isfinite(v) for v in values):raise ValueError('Nonfinite weight')
    return values

if __name__=='__main__':
    import argparse,json
    p=argparse.ArgumentParser();p.add_argument('--patterns');p.add_argument('--output');p.add_argument('--mnist');a=p.parse_args()
    if a.mnist:print(json.dumps(check_mnist(a.mnist),indent=2))
    else:print(generate_noise(a.patterns,a.output))
