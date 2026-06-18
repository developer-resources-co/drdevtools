// Minimal VS Code debug extension: registers the "drmon" debug type and launches
// the drmon DAP adapter as an executable, passing host/port/symbols as CLI args
// (the adapter reads those from argv, not from the DAP launch/attach config).
//
// No build step — plain JS. Load it either by symlinking this folder into
// ~/.vscode/extensions/ (then Reload Window), or via:
//   code --extensionDevelopmentPath=<this folder>
const vscode = require('vscode');

function activate(context) {
  const factory = {
    createDebugAdapterDescriptor(session) {
      const c = session.configuration || {};
      const bin = c.binary || '/tmp/drmon-build/drmon-dap-snes';
      const args = ['--host', String(c.host || '127.0.0.1'),
                    '--port', String(c.port || 41816)];
      if (c.symbols) args.push('--symbols', String(c.symbols));
      return new vscode.DebugAdapterExecutable(bin, args);
    }
  };
  context.subscriptions.push(
    vscode.debug.registerDebugAdapterDescriptorFactory('drmon', factory)
  );
}

function deactivate() {}

module.exports = { activate, deactivate };
