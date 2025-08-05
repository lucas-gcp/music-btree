import numpy as np
import os
import subprocess
import shutil
from pathlib import Path



with open('compositores.txt', encoding = "utf8") as f:
    COMPOSITORES = [c[:-1] for c in f.readlines()]
CATEGORIAS = ["Piano Concerto", "Violin Sonata", "String Quartet", "Symphony", "Piano Sonata", "Prelude"]

NUM_RECORDINGS = [50000]
NUM_COMPOSICOES = 18
NUM_COMPOSICOES_POR_CATEGORIA = int(NUM_COMPOSICOES / (len(COMPOSITORES) * len(CATEGORIAS)) + 1)

rng = np.random.default_rng(seed=42)
n, p = 20, (5_000_000/1_200_000) / 20 # https://support.apple.com/en-vn/guide/apple-music-classical/dev02bc3b832/web

recordings = 10001
for total in NUM_RECORDINGS:
    print(total)
    while recordings <= total:
        k = rng.binomial(n, p)

        if (k == 0):
            k = 1

        os.mkdir(f"fake-music/album{recordings}")
        with open(f"fake-music/album{recordings}/album{recordings}.txt", "w", encoding = "utf8") as f:
            s = ""
            for catalogo in rng.integers(low=1, high=NUM_COMPOSICOES_POR_CATEGORIA * len(CATEGORIAS), size=k, endpoint=True):
                compositor = rng.choice(COMPOSITORES)
                categoria = int(catalogo / (NUM_COMPOSICOES_POR_CATEGORIA + 1))
                idx = (catalogo - 1) % (NUM_COMPOSICOES_POR_CATEGORIA) + 1

                s += f"{compositor},{CATEGORIAS[categoria]} No. {idx},{catalogo}\n"

            f.write(s[:-1])
            f.write("\n")

        recordings += 1

    subprocess.run(["src/benchmark", "fake-music"])
    print("B Tree size: ", os.path.getsize('btree.db'))
    
    root_directory = Path('index')
    print("B+ Tree size: ", sum(f.stat().st_size for f in root_directory.glob('**/*') if f.is_file()))

    os.remove("btree.db")
    shutil.rmtree("B_benchmark_db")
    shutil.rmtree("BP_benchmark_db")
    shutil.rmtree("index")
