# ibm-bob/mode-pack/scripts/json_schema_lite.py
# Implements a tiny JSON Schema subset validator for the IBM-Bob packet schemas used in local tests.
# This exists so the repo can validate generated packets without adding a third-party dependency.
# RELEVANT FILES: ibm-bob/mode-pack/scripts/validate_mode_pack.py, ibm-bob/mode-pack/scripts/evaluate_mode_pack.py, ibm-bob/mode-pack/schemas/run-request.schema.json
from __future__ import annotations

from pathlib import Path

from runtime_common import load_jsonc


class LiteSchemaValidator:
    def __init__(self) -> None:
        self._cache: dict[Path, dict] = {}

    def load(self, path: Path) -> dict:
        path = path.resolve()
        if path not in self._cache:
            self._cache[path] = load_jsonc(path)
        return self._cache[path]

    def validate_file(self, data: dict, schema_path: Path) -> list[str]:
        schema = self.load(schema_path)
        return self.validate(data, schema, schema_path.parent, "$")

    def validate(self, data: object, schema: dict, base_dir: Path, at: str) -> list[str]:
        if "$ref" in schema:
            ref_path = (base_dir / schema["$ref"]).resolve()
            return self.validate(data, self.load(ref_path), ref_path.parent, at)

        errors: list[str] = []
        if "const" in schema and data != schema["const"]:
            errors.append(f"{at}: expected const {schema['const']!r}, got {data!r}")
        if "enum" in schema and data not in schema["enum"]:
            errors.append(f"{at}: expected one of {schema['enum']!r}, got {data!r}")

        schema_type = schema.get("type")
        if schema_type == "object":
            if not isinstance(data, dict):
                return [f"{at}: expected object, got {type(data).__name__}"]
            required = schema.get("required", [])
            for key in required:
                if key not in data:
                    errors.append(f"{at}: missing required key {key!r}")
            properties = schema.get("properties", {})
            if schema.get("additionalProperties") is False:
                for key in data:
                    if key not in properties:
                        errors.append(f"{at}: unexpected key {key!r}")
            for key, value in data.items():
                if key in properties:
                    errors.extend(
                        self.validate(value, properties[key], base_dir, f"{at}.{key}")
                    )
            return errors

        if schema_type == "array":
            if not isinstance(data, list):
                return [f"{at}: expected array, got {type(data).__name__}"]
            item_schema = schema.get("items")
            if item_schema:
                for index, value in enumerate(data):
                    errors.extend(
                        self.validate(value, item_schema, base_dir, f"{at}[{index}]")
                    )
            return errors

        if schema_type == "string" and not isinstance(data, str):
            errors.append(f"{at}: expected string, got {type(data).__name__}")
        if schema_type == "integer" and not isinstance(data, int):
            errors.append(f"{at}: expected integer, got {type(data).__name__}")
        if schema_type == "boolean" and not isinstance(data, bool):
            errors.append(f"{at}: expected boolean, got {type(data).__name__}")
        return errors
