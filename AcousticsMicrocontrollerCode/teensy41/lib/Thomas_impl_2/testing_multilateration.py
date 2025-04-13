import numpy as np
from scipy.optimize import least_squares

def estimate_position(hydrophones, tdoas, c=1500.0):
    """
    Estimates 3D source position from TDOAs using linear least squares + nonlinear refinement.

    Parameters:
    - hydrophones: (N, 3) array of hydrophone positions. First one is the reference.
    - tdoas: (N-1,) array of TDOAs relative to the reference hydrophone (sensor 0).
    - c: Speed of sound in the medium (default 1500 m/s for water).

    Returns:
    - Estimated 3D position (x, y, z)
    """
    p0 = hydrophones[0]
    others = hydrophones[1:]
    delta_t = np.array(tdoas)

    # Step 1: Linear least-squares estimate
    A = others - p0  # (N-1, 3)

    b = 0.5 * (
        np.sum(others**2, axis=1) - np.sum(p0**2) - (c**2) * (delta_t**2)
    )

    x_linear = np.linalg.lstsq(A, b, rcond=None)[0]  # Initial estimate

    print(x_linear)

    # Step 2: Nonlinear refinement
    def tdoa_residuals(x):
        dists = np.linalg.norm(hydrophones - x, axis=1)
        pred_tdoas = dists[1:] - dists[0]
        return pred_tdoas - c * delta_t

    result = least_squares(tdoa_residuals, x_linear)
    return result.x




hydrophones = np.array([
    [0.0, 0.0, 0.0],    # reference hydrophone
    [1.0, 1.0, 1.0],
    [1.2, 0.8, 1.1],
    [0.9, 1.3, 1.2],
    [1.1, 1.4, 0.9]
])

# Simulated TDOA values relative to hydrophone 0 (in seconds)
tdoas = np.array([
-0.0006967227, -0.0006985525, -0.0008852526, -0.0006837616
])

position = estimate_position(hydrophones, tdoas, c=1500.0)
print("Estimated position:", position)
