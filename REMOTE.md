# Publishing this repo (persona GitHub account)

Persona: **BayMaxMaxMax**  
SSH remote (already configured):

`git@github.com-baymax:BayMaxMaxMax/quant-review-demo.git`

This uses `~/.ssh/id_ed25519_baymax` via Host `github.com-baymax`.  
Default `github.com` remains the Celine key so other repos are unaffected.

## Before first push

1. Add `~/.ssh/id_ed25519_baymax.pub` to https://github.com/settings/keys (while logged in as BayMaxMaxMax)
2. Create empty public repo `quant-review-demo` under BayMaxMaxMax (no README), then:

```bash
cd /Users/dzhuang/projects/quant-review-demo
ssh -T git@github.com-baymax
git push -u origin main
```

Private ops checklist:  
`AI-Learning/08-side-business/02-quant-ai-knowledge-consulting/github-remote-setup.md`
