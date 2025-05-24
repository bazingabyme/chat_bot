import importlib.util
import sys
from fastapi import FastAPI

# Load the .pyc file
# spec = importlib.util.spec_from_file_location("NEWIP99003_fastAPI", "__pycache__/NEWIP99003_fastAPI.cpython-311.pyc")
spec = importlib.util.spec_from_file_location("IP99003v2", "IP99003v2.cp311-win_amd64.pyd")
module = importlib.util.module_from_spec(spec)
sys.modules["IP99003v2"] = module
spec.loader.exec_module(module)

app = module.app

if __name__ == "__main__":
    import uvicorn
    uvicorn.run(app, host="0.0.0.0", port=9091)
