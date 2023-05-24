import matplotlib.pyplot as plt
import matplotlib.gridspec as gridspec

# Make a good layout ==================================================
fig = plt.figure()
# Create an outer GridSpec for the two columns
outer_gs = gridspec.GridSpec(1, 2, figure=fig, width_ratios=[1, 1])
# Create an inner GridSpec for the first column
gs_hydrophone = gridspec.GridSpecFromSubplotSpec(5, 1, subplot_spec=outer_gs[0], hspace=0.1)
# Create an inner GridSpec for the second column, with height ratios for the 70%/30% split
gs_dsp = gridspec.GridSpecFromSubplotSpec(2, 1, subplot_spec=outer_gs[1], height_ratios=[7, 3])

hydrophoneAxis = [None]*5

# Add subplots in the first column for hydrophone data
for i in range(5):
    hydrophoneAxis[i] = fig.add_subplot(gs_hydrophone[i, 0], sharex=hydrophoneAxis[0] if i else None)
    hydrophoneAxis[i].label_outer()
fig.text(0.25, 0.94, 'Hydrophone Data', ha='center')

# Add subplots in the second column
FFTAxis = fig.add_subplot(gs_dsp[0])
FFTAxis.set_title('FFT')

filterAxis = fig.add_subplot(gs_dsp[1])
filterAxis.set_title('Filter response')

plt.tight_layout()
plt.show()
