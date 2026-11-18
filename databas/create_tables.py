import sqlite3
import subprocess
import sys
from pathlib import Path


script_dir = Path(__file__).resolve().parent
db_path = script_dir / "stars.db"


db = sqlite3.connect(db_path)
cur = db.cursor()


cur.execute("""
CREATE TABLE IF NOT EXISTS stars(
    id INTEGER PRIMARY KEY,
    name TEXT,
    ra REAL,
    dec REAL,
    parallax REAL,
    v_mag REAL,
    b_mag REAL,
    spectral_type TEXT
)
""")


cur.execute("""
CREATE TABLE IF NOT EXISTS users(
    id INTEGER PRIMARY KEY,
    name TEXT UNIQUE NOT NULL,
    password TEXT NOT NULL,
    isAdmin INTEGER NOT NULL DEFAULT 0
)
""")


cur.execute("""
CREATE TABLE IF NOT EXISTS favorites(
    userId INTEGER NOT NULL,
    starId INTEGER NOT NULL,
    PRIMARY KEY (userId, starId),
    FOREIGN KEY (userId) REFERENCES users(id),
    FOREIGN KEY (starId) REFERENCES stars(id)
)
""")


db.commit()
db.close()

print(f"Created tables in {db_path}: stars, users, favorites")

subprocess.run([sys.executable, "stjarn_import.py"], cwd=script_dir, check=True)
