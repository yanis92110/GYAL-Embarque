from pathlib import Path
import sys,unittest,random,json
ROOT=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(ROOT))
from tools import DIGITS,read_pattern,noisy_copy,read_weights
from gradient_check import check_gradient

class DataTests(unittest.TestCase):
    def test_patterns(self):
        patterns=[]
        for digit,name in enumerate(DIGITS):
            x,y=read_pattern(ROOT/'patterns'/f'{name}.txt');self.assertEqual(y,digit);patterns.append(tuple(x))
            for p in range(0,101,10):self.assertEqual(sum(a!=b for a,b in zip(x,noisy_copy(x,p,random.Random(2026)))),(48*p+50)//100)
        self.assertEqual(len(set(patterns)),10)
    def test_reader(self):
        for seed in [42,43,44]:self.assertEqual(len(read_weights(ROOT/'weights'/f'seed{seed}.bin')),79510)
    def test_finite_difference_tool(self):
        # This tests the checker, NOT your backpropagation.
        checks=check_gradient([.1,-.3],lambda t:sum(v*v for v in t),lambda t:[2*v for v in t])
        self.assertTrue(all(r['passed'] for r in checks))

if __name__=='__main__':unittest.main()
