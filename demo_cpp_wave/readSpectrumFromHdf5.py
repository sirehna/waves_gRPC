import h5py
f=h5py.File('waves.hdf5')
spectrum00 = f['outputs/spectra/00']

def maptest(foo):
    return str(foo[0])

for key in list(spectrum00.keys()):
    with open('out.txt', 'a') as fo:
        print(key, '{', ', '.join(list(map(maptest, spectrum00[key].value))), '}', file=fo)