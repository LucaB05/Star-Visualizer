import sqlite3
from astroquery.simbad import Simbad #ni måste ladda ned astroquery och astropy på er dator om ni ska köra scriptet


db = sqlite3.connect("stars.db")
cur = db.cursor()

# Added IF EXISTS to fix error if dosent exist
cur.execute("""
drop TABLE IF EXISTS stars
""")

print("TABLE stars deleted")

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
db.commit()


simbad_object = Simbad()

#'*..' på rad 32 tar med alla stjärnor. t.ex 'Star' tar bara med stjärnor som inte har något mer specifik kategorisering som t.ex "Flare star"
adql_query = """
SELECT TOP 1000 b.main_id, b.ra, b.dec, b.plx_value, b.sp_type,
f.V as v_mag, f.b as b_mag
FROM basic AS b
LEFT JOIN allfluxes AS f ON b.oid = f.oidref
WHERE plx_value > 0 
    AND (b.otype = 'Star' 
    OR b.otype = '**' 
    OR b.otype = 'V*' 
    OR b.otype = 'WD*' 
    OR b.otype = 'PM*')
ORDER BY plx_value DESC
"""


stars_table = simbad_object.query_tap(adql_query)

# lägger in i tabellen
for star in stars_table:
    name = star['main_id']
    if name.startswith("NAME "):
        name = name.replace("NAME ", "", 1)
    ra = float(star['ra'])
    dec = float(star['dec'])
    parallax = float(star['plx_value'])

    
    if 'v_mag' in star.colnames and star['v_mag'] is not None:
        v_mag = float(star['v_mag'])
    else:
        v_mag = None
    
    if 'b_mag' in star.colnames and star['b_mag'] is not None:
        b_mag = float(star['b_mag'])
    else:
        b_mag = None

   
    if star['sp_type'] not in (None, ''):
        spectral_type = star['sp_type']
    else:
        spectral_type = None
    


    cur.execute("""
        INSERT INTO stars (name, ra, dec, parallax, v_mag, b_mag, spectral_type)
        VALUES (?, ?, ?, ?, ?, ?, ?)
    """, (name, ra, dec, parallax, v_mag, b_mag, spectral_type))

db.commit()
db.close()

print(f"Inserted {len(stars_table)} stars into orbit (starts.db)")