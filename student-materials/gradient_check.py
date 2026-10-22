"""Centered differences. Supply YOUR loss and gradients; no backprop here."""
def check_gradient(theta,loss_fn,gradient_fn,h=1e-5):
    analytical=gradient_fn(theta[:]);rows=[]
    if len(analytical)!=len(theta):raise ValueError('Gradient length mismatch')
    for i,g in enumerate(analytical):
        plus=theta[:];minus=theta[:];plus[i]+=h;minus[i]-=h
        numerical=(loss_fn(plus)-loss_fn(minus))/(2*h)
        limit=1e-6+1e-3*max(abs(g),abs(numerical))
        rows.append(dict(index=i,analytical=g,numerical=numerical,error=abs(g-numerical),tolerance=limit,passed=abs(g-numerical)<=limit))
    return rows

def gradients_from_step(theta,step_fn,eta=.01):
    updated=step_fn(theta[:],eta)
    return [(before-after)/eta for before,after in zip(theta,updated)]
