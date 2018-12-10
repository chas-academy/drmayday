workflow "Test on PR" {
  on = "pull_request"
  resolves = ["Test"]
}

action "Test" {
  uses = "actions/npm@6309cd9"
  args = "test"
}

workflow "Test on Push" {
  on = "push"
  resolves = ["GitHub Action for npm"]
}

action "GitHub Action for npm" {
  uses = "actions/npm@6309cd9"
  args = "test"
}
