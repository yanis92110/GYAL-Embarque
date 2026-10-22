"""Exercise: this update has a bug. Prove it using the supplied checker."""
def update(theta,gradient,eta):
    result=theta[:]
    # The last two entries were thought to be metadata. Check that assumption.
    for i in range(len(theta)-2):
        result[i]-=eta*gradient[i]
    return result
