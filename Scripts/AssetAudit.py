import csv
import os

import unreal


OUTPUT_PATH = os.path.join(
    unreal.Paths.project_saved_dir(), "Reports", "VisualBenchmarkAssetAudit.csv"
)
ROOTS = ["/Game/AnimeCitySuburbs", "/Game/Cartoon_City_Free"]


def safe_value(callable_value, fallback=-1):
    try:
        return callable_value()
    except Exception:
        return fallback


registry = unreal.AssetRegistryHelpers.get_asset_registry()
rows = []

for root in ROOTS:
    for asset_data in registry.get_assets_by_path(root, recursive=True):
        if str(asset_data.asset_class_path.asset_name) != "StaticMesh":
            continue

        mesh = asset_data.get_asset()
        if not mesh:
            continue

        bounds = mesh.get_bounds()
        extent = bounds.box_extent
        rows.append(
            {
                "pack": root.rsplit("/", 1)[-1],
                "name": str(asset_data.asset_name),
                "path": str(asset_data.package_name),
                "size_x_cm": round(extent.x * 2.0, 2),
                "size_y_cm": round(extent.y * 2.0, 2),
                "size_z_cm": round(extent.z * 2.0, 2),
                "lod_count": safe_value(lambda: unreal.EditorStaticMeshLibrary.get_lod_count(mesh)),
                "vertex_count_lod0": safe_value(lambda: unreal.EditorStaticMeshLibrary.get_number_verts(mesh, 0)),
                "convex_collision_count": safe_value(lambda: unreal.EditorStaticMeshLibrary.get_convex_collision_count(mesh)),
            }
        )

rows.sort(key=lambda row: (row["pack"], row["path"]))
os.makedirs(os.path.dirname(OUTPUT_PATH), exist_ok=True)
with open(OUTPUT_PATH, "w", newline="", encoding="utf-8-sig") as output_file:
    writer = csv.DictWriter(output_file, fieldnames=list(rows[0].keys()))
    writer.writeheader()
    writer.writerows(rows)

unreal.log(f"[BDRAssetAudit] Wrote {len(rows)} static meshes to {OUTPUT_PATH}")
