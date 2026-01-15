local dap = require("dap")

dap.configurations.cpp = {
    {
        name = "Runner",
        type = "codelldb",
        request = "launch",
        program = vim.fn.getcwd() .. "/build/Debug/src/runner",
        cwd = vim.fn.getcwd(),
        args = function()
           local input_args = vim.fn.input("Args: ")
            return vim.split(input_args, " ", {trimempty=true}) 
        end,
        console = "integratedTerminal"
    }
}
