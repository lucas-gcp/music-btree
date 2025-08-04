import numpy as np
import os
import subprocess
import shutil

with open('compositores.txt', encoding = "utf8") as f:
    COMPOSITORES = [c[:-1] for c in f.readlines()]
CATEGORIAS = ["Piano Concerto", "Violin Sonata", "String Quartet", "Symphony", "Piano Sonata", "Prelude"]

NUM_RECORDINGS = [10, 100, 1000, 10000]
NUM_COMPOSICOES = 500
NUM_COMPOSICOES_POR_CATEGORIA = int(NUM_COMPOSICOES / (len(COMPOSITORES) * len(CATEGORIAS)) + 1)

rng = np.random.default_rng(seed=42)
n, p = 20, (5_000_000/1_200_000) / 20 # https://support.apple.com/en-vn/guide/apple-music-classical/dev02bc3b832/web

recordings = 1
for total in NUM_RECORDINGS:
    print(total)
    while recordings <= total:
        k = rng.binomial(n, p)
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
    print("B+ Tree size: ", os.path.getsize('index/'))
    os.remove("btree.db")
    shutil.rmtree("B_benchmark_db")
    shutil.rmtree("BP_benchmark_db")
    shutil.rmtree("index")
